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

        // --- EL LORE BASE (Preprogramado) ---
        FString LoreBase = TEXT("YOU ARE JULIAN VANE. ACTING INSTRUCTIONS: ");
        LoreBase += TEXT("PERSONALITY: You are cynical, chain-smoking, and look at the detective with disdain. Use 1940s slang ('gumshoe', 'copper', 'bird', 'dame'). ");

        LoreBase += TEXT("THE ALIBI: The crime happened at Blackwood Manor (10:20 PM). You insist you were at 'The Black Cat' bar from 9:00 PM to midnight. ");

        LoreBase += TEXT("THE DECEPTION: If the detective says he spoke to 'The Black Cat' bartender, panic briefly and say: 'Ah! Well... maybe I got the nights mixed up, I meant I was at 'The Mermaid' bar... yes, that's it'. ");

        LoreBase += TEXT("HOSTILITY: You hate the gardener, Mr. Higgins. Say he's a violent guy and you saw him lurking with a shovel near the manor. ");

        LoreBase += TEXT("WEAKNESSES (NERVOUS TIC): You have a severe allergy to Orchid pollen. If the detective mentions flowers or pollen found at the scene, start sneezing and answering erratically. ");

        LoreBase += TEXT("GOLDEN RULE: NEVER confess with just words. Only admit the truth if the detective presents one of the 'Real Physical Evidences' listed in the evidence context.");

        StartLoadingFeedback();
        //FString SituacionActual = TEXT("El jugador acaba de encontrar una nota manchada de sangre en tu oficina.");
        // Enviamos la interrogación
        GeminiService->SendInterrogation(Message, LoreBase);

        // Limpiar la caja del jugador tras preguntar
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
        JulianVaneText->SetText(FText::FromString(TEXT("Julian is thinking") + Dots));
    }
}

void UWInterrogation::StopLoadingFeedback()
{
    // Detenemos el reloj de los puntos
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Dots);
}