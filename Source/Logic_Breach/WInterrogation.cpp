#include "WInterrogation.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWInterrogation::NativeConstruct()
{
    Super::NativeConstruct();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("C++: WInterrogation conectado con éxito"));
    }

    // 1. Buscamos al actor AGeminiService que está arrastrado en el mapa
    GeminiService = Cast<AGeminiService>(UGameplayStatics::GetActorOfClass(GetWorld(), AGeminiService::StaticClass()));

    if (GeminiService)
    {
        // 2. Conectamos la respuesta de la IA a nuestra función local
        GeminiService->OnJulianReplied.AddDynamic(this, &UWInterrogation::HandleJulianResponse);
    }

    // 3. Decimos que al hacer clic en el botón, se ejecute nuestra función
    if (InterrogateButton)
    {
        
        InterrogateButton->OnClicked.AddDynamic(this, &UWInterrogation::OnInterrogateClicked);
    }
}

void UWInterrogation::OnInterrogateClicked()
{
    if (GeminiService && PlayerText)
    {
        // Obtenemos lo que el jugador escribió
        FString Message = PlayerText->GetText().ToString();

        // Enviamos la interrogación (usando tu función de GeminiService)
        GeminiService->SendInterrogation(Message, TEXT("Eres Julian, un sospechoso de 1947. Responde de forma misteriosa."));

        // Opcional: Limpiar la caja del jugador tras preguntar
        PlayerText->SetText(FText::GetEmpty());
    }
}

void UWInterrogation::HandleJulianResponse(const FString& Reply)
{
    if (JulianVaneText)
    {
        UKismetSystemLibrary::PrintString(this, "RESPUESTA: " + Reply, true, true, FLinearColor::Blue, 5.f);
        JulianVaneText->SetText(FText::FromString(Reply));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Mensaje?"));
        }
    }
}