#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "GeminiService.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGeminiResponseDelegate, const FString&, Reply);

UCLASS()
class LOGIC_BREACH_API AGeminiService : public AActor
{
    GENERATED_BODY()

public:
    AGeminiService();

    UFUNCTION(BlueprintCallable, Category = "Logic Breach | AI")
    void SendInterrogation(FString PlayerMessage, FString EvidenceContext);

    UPROPERTY(BlueprintAssignable, Category = "Logic Breach | AI")
    FGeminiResponseDelegate OnJulianReplied;

private:
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    //FString ApiUrl = TEXT("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent");
};