#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GeminiService.h"
#include "WInterrogation.generated.h"



class UMultiLineEditableText;
class UEditableTextBox;
class UButton;

UCLASS()
class LOGIC_BREACH_API UWInterrogation : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UMultiLineEditableText* PlayerText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UMultiLineEditableText* JulianVaneText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UButton* InterrogateButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic Breach | Animation")
    AActor* JulianCharacterActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interrogatorio | Status")
    bool bIsJulianTalking;

    void SetTalkingVariable(float Value);

    // Referencia al servicio que se tiene en el mapa
    UPROPERTY()
    AGeminiService* GeminiService;

    // Se ejecuta cuando el Widget se crea (BeginPlay)
    virtual void NativeConstruct() override;

    // Efecto maquina de escribir

    FTimerHandle TimerHandle_Typewriter;    // El reloj que ejecutara la funcion cada X milisegundos
    FString FullResponse;      // Se guarda la respuesta completa que nos envie Gemini
    int32 CurrentLetterIndex;  // Rastrea que letra es la siguiente en imprimirse

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* JulianScrollBox;
   
    void PlayNextLetter(); // Función interna del efecto


    // El reloj para la animación de carga
    FTimerHandle TimerHandle_Dots;

    // Contador para saber cuántos puntos mostrar (0, 1, 2 o 3)
    int32 DotCount;

    // Función que se repetirá para actualizar el texto
    void UpdateLoadingDots();

    // Función para iniciar y detener la carga
    void StartLoadingFeedback();
    void StopLoadingFeedback();

private:
    // Función para el botón
    UFUNCTION()
    void OnInterrogateClicked();

    // Función que recibirá la respuesta de Julian
    UFUNCTION()
    void HandleJulianResponse(const FString& Reply);
};