#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "GeminiService.generated.h"

// Delegado para enviar la respuesta a Blueprints/UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGeminiResponseDelegate, const FString&, Reply);

UCLASS()
class LOGIC_BREACH_API AGeminiService : public AActor
{
    GENERATED_BODY()

public:
    AGeminiService();

    // Función que llamarás desde Unreal para interrogar
    UFUNCTION(BlueprintCallable, Category = "Logic Breach | AI")
    void SendInterrogation(FString PlayerMessage, FString EvidenceContext);

    // Evento para que la UI sepa cuando Julian responde
    UPROPERTY(BlueprintAssignable, Category = "Logic Breach | AI")
    FGeminiResponseDelegate OnJulianReplied;

private:
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    // Configuración de la API
    FString ApiKey = TEXT("AIzaSyAWI0P9NqnHvL-8OTY9q3AdcV5x8xcLtRQ");
    FString ApiUrl = TEXT("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-pro:generateContent?key=");
};