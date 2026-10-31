# TP_MRQuest – instrukcja

Szablon projektu **Unreal Engine 5.6** dla **Meta Quest 3 / 3S** w trybie Mixed Reality:
passthrough jako tło + śledzenie dłoni z **Meta XR Interaction SDK (ISDK)**.

## 1. Wymagania

| Składnik | Wersja |
|---|---|
| Unreal Engine | 5.6.1 (Epic Games Launcher), platforma **Android** zainstalowana |
| Meta XR Plugin | 1.117.0, w `Engine\Plugins\Marketplace\MetaXR` |
| Meta XR Interaction SDK | 1.85.0, w `Engine\Plugins\Marketplace\MetaXRInteraction` |
| Android Studio | Koala 2024.1.2 |
| Android SDK / Build-Tools | API 34 / 34.0.0 |
| Android NDK | r27c (27.2.12479018) |
| JDK | OpenJDK 21 |
| IDE | Visual Studio 2022 / Rider |
| Gogle | Meta Quest 3 / 3S – tryb dewelopera, śledzenie dłoni włączone |

### 1.1 Pluginy Meta
Oba pluginy (Meta XR i Meta XR Interaction SDK) instaluje się **do silnika** (Fab / Epic Games Launcher →
*Install to Engine* → 5.6). Nie ma ich w repozytorium (licencja Meta). Bez nich projekt z szablonu
się nie skompiluje.

### 1.2 Android
W UE: **Edit → Project Settings → Platforms → Android SDK** – ścieżki do SDK, NDK i JDK.
Gogle: tryb dewelopera w aplikacji Meta Horizon, USB debugging zaakceptowany w goglach.

## 2. Instalacja szablonu

1. Sklonuj / pobierz repozytorium.
2. PowerShell **jako administrator** w folderze repo:
   ```powershell
   Set-ExecutionPolicy -Scope Process Bypass
   .\scripts\install-template.ps1
   ```
   Inny folder silnika: `-EnginePath "D:\Epic Games\UE_5.6"`. Nadpisanie bez pytania: `-Force`.
3. Skrypt kopiuje szablon do `UE_5.6\Templates\TP_MRQuest` (bez `Binaries`, `Intermediate`, `Saved`,
   `docs`, `scripts`, plików git) i usuwa `ProjectID` z `DefaultGame.ini`.
4. Uruchom ponownie Unreal Engine.

## 3. Nowy projekt

1. **New Project → Games → MR Quest 3 (Passthrough + Hands)** → nazwa, lokalizacja → **Create**.
2. Unreal zamienia `TP_MRQuest` na nazwę projektu w klasach C++, module, `.ini` i `.cs`
   (np. `ATP_MRQuestPawn` → `AMojProjektPawn`). Blueprinty dostają przekierowanie
   (`ActiveGameNameRedirects`), więc `BP_MRPawn` / `BP_MRGameMode` mają poprawnych rodziców.
3. Przed publikacją zmień **Project Settings → Android → Package Name** (`com.YourCompany.[PROJECT]`).

## 4. Uruchomienie na Queście

1. Gogle po USB, zaakceptuj debugowanie.
2. **Platforms → Quest 3 → Launch** (albo *Package Project → Android* i instalacja APK).
3. Efekt: widzisz pokój (passthrough) i swoje dłonie (odłóż kontrolery).

## 5. Co jest w szablonie

- **`ATP_MRQuestPawn`** (C++): `VROrigin` → `Camera` (HMD), `MotionControllerLeft/Right` →
  rigi dłoni ISDK (`UIsdkHandRigComponentLeft/Right`, grab / poke / ray).
  - `TrackingOrigin` = `LocalFloor` (Z = 0 na prawdziwej podłodze),
  - `IMC_IsdkHand` (z pluginu ISDK) dodawany w `NotifyControllerChanged` – bez niego pinch nie działa,
  - persistent passthrough (Underlay) w `BeginPlay`; `bStartPassthrough`, `PassthroughOpacity` w Class Defaults.
- **`ATP_MRQuestGameMode`** (C++) + **`BP_MRGameMode`** (Default Pawn = `BP_MRPawn`).
- **`L_MR`**: pusta mapa startowa – tylko światło i PlayerStart.
- **Config**: Vulkan, Arm64, Meta Quest 3/3S, `HandsOnly`, passthrough włączony, forward shading + MSAA,
  `r.PostProcessing.PropagateAlpha=True`.
- **Guardian ukryty**: `bBoundaryVisibilitySupportEnabled=True` + `bDefaultBoundaryVisibilitySuppressed=True`
  (Project Settings → Meta XR → *Boundary Visibility Support* / *Default Is Boundary Visibility Suppressed*).
  Działa tylko z włączonym passthrough; dodaje do manifestu `com.oculus.permission.BOUNDARY_VISIBILITY`.
  Chcesz siatkę z powrotem – odznacz *Default Is Boundary Visibility Suppressed*.

Rozbudowuj `BP_MRPawn` / `BP_MRGameMode` albo klasy C++. Assety trzymaj w `Content/MRTemplate/`
lub własnych folderach.

## 6. Problemy

| Objaw | Przyczyna / rozwiązanie |
|---|---|
| Czarne tło zamiast pokoju | Brak `r.PostProcessing.PropagateAlpha=True` w `DefaultEngine.ini` albo Sky / Fog / Cloud na mapie |
| Latająca kula zamiast dłoni | Default GameMode ≠ `BP_MRGameMode` (Project Settings → Maps & Modes) |
| Podwójne dłonie | Dodany komponent `Oculus XR Hand` obok rigu ISDK – usuń go |
| Brak dłoni | Śledzenie dłoni wyłączone w goglach (Ustawienia → Śledzenie ruchu → Śledzenie dłoni) |
| Szablonu nie ma w New Project | Restart edytora; sprawdź `UE_5.6\Templates\TP_MRQuest` |
| Projekt się nie kompiluje | Brak pluginu Meta XR lub Meta XR Interaction SDK w silniku |

Logi: `adb logcat -s UE`, kategoria `Log<NazwaProjektu>`:
`Persistent passthrough initialized`, `Left/Right hand rig ready`, `Hand input mapping context added`.
