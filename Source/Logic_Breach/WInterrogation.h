#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GeminiService.h" // Importamos tu servicio de IA
#include "WInterrogation.generated.h"

// Declaramos que usaremos estas clases para que el compilador no se queje
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
    class UEditableTextBox* PlayerText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UEditableTextBox* JulianVaneText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Interrogatorio")
    class UButton* InterrogateButton;

    // Referencia al servicio que ya tienes en el mapa
    UPROPERTY()
    AGeminiService* GeminiService;

    // Se ejecuta cuando el Widget se crea (como el BeginPlay)
    virtual void NativeConstruct() override;

private:
    // Función para el botón
    UFUNCTION()
    void OnInterrogateClicked();

    // Función que recibirá la respuesta de Julian
    UFUNCTION()
    void HandleJulianResponse(const FString& Reply);
};