#include "GeminiService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"

AGeminiService::AGeminiService()
{
    PrimaryActorTick.bCanEverTick = false;
    
}

void AGeminiService::SendInterrogation(FString PlayerMessage, FString EvidenceContext)
{
    UE_LOG(LogTemp, Error, TEXT("TEST"));
    // --- CARGA SEGURA DE API KEY --- 
    FString LocalApiKey;
    FString ConfigFilePath = FPaths::ProjectConfigDir() + TEXT("SecretKeys.ini");

    FConfigCacheIni::NormalizeConfigIniPath(ConfigFilePath);

    bool bFound = GConfig->GetString(
        TEXT("/Script/SecretSettings"),
        TEXT("GoogleApiKey"),
        LocalApiKey,
        ConfigFilePath
    );

    if (!bFound || LocalApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("AI_ERROR: No se pudo cargar la API Key desde %s. Revisa tu archivo .ini"), *ConfigFilePath);
        return;
    }

    // --- CONFIGURACIÓN DE PETICIÓN ---
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AGeminiService::OnResponseReceived);

    FString ModelName = TEXT("gemini-2.5-flash"); // Más estable que el 'preview'
    FString FullUrl = FString::Printf(TEXT("https://generativelanguage.googleapis.com/v1beta/models/%s:generateContent?key=%s"), *ModelName, *LocalApiKey);

    Request->SetURL(FullUrl);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    UE_LOG(LogTemp, Log, TEXT("URL de envío: %s"), *FullUrl);

    // --- CONSTRUCCIÓN DEL JSON ---
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> ContentsArray;

    TSharedPtr<FJsonObject> ContentObject = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> PartsArray;
    TSharedPtr<FJsonObject> TextPart = MakeShareable(new FJsonObject());

    // 1. System Instruction
    TSharedPtr<FJsonObject> SystemInstObj = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> SysPartsArray;
    TSharedPtr<FJsonObject> SysTextObj = MakeShareable(new FJsonObject());

    FString JulianPersona = TEXT("Eres Julian Vane, archivista del museo en 1947. ");
    JulianPersona += TEXT("Eres culto, hablas con elegancia Noir. Eres sospechoso de robar el Zafiro. ");
    JulianPersona += TEXT("Niega el robo pero muestra nerviosismo si mencionan pruebas. ");
    JulianPersona += TEXT("Contexto de pruebas actuales: ") + EvidenceContext;

    SysTextObj->SetStringField(TEXT("text"), JulianPersona);
    SysPartsArray.Add(MakeShareable(new FJsonValueObject(SysTextObj)));
    SystemInstObj->SetArrayField(TEXT("parts"), SysPartsArray);
    RootObject->SetObjectField(TEXT("system_instruction"), SystemInstObj);

    // 2. Contents
    TSharedPtr<FJsonObject> ContentObj = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> UserPartsArray;
    TSharedPtr<FJsonObject> UserTextObj = MakeShareable(new FJsonObject());

    FString FullPrompt = FString::Printf(TEXT("%s. El jugador pregunta: %s"), *EvidenceContext, *PlayerMessage);

    TextPart->SetStringField(TEXT("text"), FullPrompt);
    PartsArray.Add(MakeShareable(new FJsonValueObject(TextPart)));

    ContentObject->SetArrayField(TEXT("parts"), PartsArray);
    ContentObject->SetStringField(TEXT("role"), TEXT("user"));

    ContentsArray.Add(MakeShareable(new FJsonValueObject(ContentObject)));

    RootObject->SetArrayField(TEXT("contents"), ContentsArray);

    // Serializar y Enviar
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);
    Request->ProcessRequest();

    UE_LOG(LogTemp, Log, TEXT("Petición enviada a: %s"), *FullUrl);
}

void AGeminiService::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("OnResponseReceived"));
    }

    if (bWasSuccessful && Response.IsValid())
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            const TArray<TSharedPtr<FJsonValue>>* Candidates;
            if (JsonObject->TryGetArrayField(TEXT("candidates"), Candidates) && Candidates->Num() > 0)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TryGetArray"));
                TSharedPtr<FJsonObject> Content = (*Candidates)[0]->AsObject()->GetObjectField(TEXT("content"));
                TArray<TSharedPtr<FJsonValue>> Parts = Content->GetArrayField(TEXT("parts"));
                FString FinalReply = Parts[0]->AsObject()->GetStringField(TEXT("text"));

                OnJulianReplied.Broadcast(FinalReply);
                UE_LOG(LogTemp, Log, TEXT("Julian: %s"), *FinalReply);   
            }
            else {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Algo fallo bro."));
                // Esto te dirá exactamente qué error tiene Google (ej: "API_KEY_INVALID" o "MODEL_NOT_FOUND")
                FString ErrorResponse = Response->GetContentAsString();
                GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Google Error: %s"), *ErrorResponse));
                UE_LOG(LogTemp, Error, TEXT("Cuerpo del error: %s"), *ErrorResponse);
            }
        }
    }
    else
    {
        if (GEngine)
        {
            
        }
    }
}