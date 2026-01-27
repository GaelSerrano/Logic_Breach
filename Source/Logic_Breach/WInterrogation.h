#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GeminiService.h" // Importamos tu servicio de IA
#include "WInterrogation.generated.h"


// Declaramos que usaremos estas clases para que el compilador no se queje
class UMultiLineEditableText;
class UEditableTextBox;
class UButton;

UCLASS()
class LOGIC_BREACH_API UWInterrogation : public UUserWidget
{
    GENERATED_BODY()

protected:
    // BlueprintReadOnly permite que los nodos de Blueprint vean la variable.
    // Category ayuda a organizar los nodos en el buscador.
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UMultiLineEditableText* PlayerText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UMultiLineEditableText* JulianVaneText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UButton* InterrogateButton;

    // Referencia al servicio que tenemos en el mapa
    UPROPERTY()
    AGeminiService* GeminiService;

    // Se ejecuta cuando el Widget se crea (BeginPlay)
    virtual void NativeConstruct() override;

    // --- Variables para el efecto Máquina de Escribir ---

    FTimerHandle TimerHandle_Typewriter;    // El reloj que ejecutara la funcion cada X milisegundos
    FString FullResponse;      // Se guarda la respuesta completa que nos envie Gemini
    int32 CurrentLetterIndex;  // Rastrea que letra es la siguiente en imprimirse

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* JulianScrollBox;
   
    void PlayNextLetter(); // --- Función interna del efecto ---


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