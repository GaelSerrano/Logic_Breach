#include "GeminiService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

AGeminiService::AGeminiService()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGeminiService::SendInterrogation(FString PlayerMessage, FString EvidenceContext)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AGeminiService::OnResponseReceived);

    Request->SetURL(ApiUrl + ApiKey);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // --- CONSTRUCCIÓN DEL JSON ---
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());

    // 1. System Instruction (La personalidad de Julian)
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

    // 2. Contents (El mensaje del jugador)
    TArray<TSharedPtr<FJsonValue>> ContentsArray;
    TSharedPtr<FJsonObject> ContentObj = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> UserPartsArray;
    TSharedPtr<FJsonObject> UserTextObj = MakeShareable(new FJsonObject());

    UserTextObj->SetStringField(TEXT("text"), PlayerMessage);
    UserPartsArray.Add(MakeShareable(new FJsonValueObject(UserTextObj)));
    ContentObj->SetArrayField(TEXT("parts"), UserPartsArray);
    ContentsArray.Add(MakeShareable(new FJsonValueObject(ContentObj)));
    RootObject->SetArrayField(TEXT("contents"), ContentsArray);

    // Serializar y Enviar
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);
    Request->ProcessRequest();
}

void AGeminiService::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            // Navegación: candidates[0] -> content -> parts[0] -> text
            const TArray<TSharedPtr<FJsonValue>>* Candidates;
            if (JsonObject->TryGetArrayField(TEXT("candidates"), Candidates) && Candidates->Num() > 0)
            {
                TSharedPtr<FJsonObject> Content = (*Candidates)[0]->AsObject()->GetObjectField(TEXT("content"));
                TArray<TSharedPtr<FJsonValue>> Parts = Content->GetArrayField(TEXT("parts"));
                FString FinalReply = Parts[0]->AsObject()->GetStringField(TEXT("text"));

                // Lanzar el evento para la UI o el Log
                OnJulianReplied.Broadcast(FinalReply);
                UE_LOG(LogTemp, Log, TEXT("Julian: %s"), *FinalReply);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Error en la petición a Gemini"));
    }
}