#include "WInterrogation.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/MultiLineEditableText.h"
#include "TimerManager.h"
#include "Components/ScrollBox.h"

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

        StartLoadingFeedback();
        FString SituacionActual = TEXT("El jugador acaba de encontrar una nota manchada de sangre en tu oficina.");
        // Enviamos la interrogación (usando tu función de GeminiService)
        GeminiService->SendInterrogation(Message, TEXT("Eres Julian, un sospechoso de 1947. Responde de forma misteriosa."));

        // Opcional: Limpiar la caja del jugador tras preguntar
        PlayerText->SetText(FText::GetEmpty());
    }
}

void UWInterrogation::HandleJulianResponse(const FString& Reply)
{
    StopLoadingFeedback();

    if (JulianVaneText)
    {
        FullResponse = Reply;
        CurrentLetterIndex = 0;

        //UKismetSystemLibrary::PrintString(this, "RESPUESTA: " + Reply, true, true, FLinearColor::Blue, 5.f);
        JulianVaneText->SetText(FText::GetEmpty());
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_Typewriter,
            this,
            &UWInterrogation::PlayNextLetter,
            0.04f,
            true
        );
    }
}

void UWInterrogation::PlayNextLetter()
{
    // Comprobamos si aún quedan letras por escribir
    if (CurrentLetterIndex < FullResponse.Len())
    {
        CurrentLetterIndex++;

        // Cortamos el texto desde el inicio hasta la letra actual
        FString CurrentText = FullResponse.Left(CurrentLetterIndex);

        // Lo mostramos en el Widget
        JulianVaneText->SetText(FText::FromString(CurrentText));

        // Forzamos al ScrollBox a bajar automáticamente para seguir el texto
        if (JulianScrollBox)
        {
            JulianScrollBox->ScrollToEnd();
        }
    }
    else
    {
        // Si ya terminamos de escribir todo, apagamos el cronómetro
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);
    }
}


void UWInterrogation::StartLoadingFeedback()
{
    DotCount = 0;
    // Iniciamos un timer que llama a UpdateLoadingDots cada 0.5 segundos
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Dots, this, &UWInterrogation::UpdateLoadingDots, 0.5f, true);
}

void UWInterrogation::UpdateLoadingDots()
{
    DotCount = (DotCount + 1) % 4; // Ciclo de 0 a 3
    FString Dots = TEXT("");

    for (int i = 0; i < DotCount; i++) {
        Dots += TEXT(".");
    }

    if (JulianVaneText)
    {
        JulianVaneText->SetText(FText::FromString(TEXT("Julian está pensando") + Dots));
    }
}

void UWInterrogation::StopLoadingFeedback()
{
    // Detenemos el reloj de los puntos
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Dots);
}