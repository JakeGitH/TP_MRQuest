# TP_MRQuest – Mixed Reality template for Meta Quest 3 / 3S (Unreal Engine 5.6)

A C++ project template for **Unreal Engine 5.6** that starts every new project as a ready-to-run
**Mixed Reality** app for **Meta Quest 3 / 3S**:

- **Passthrough** – the real room as the background (persistent passthrough, underlay),
- **Hand tracking** with **Meta XR Interaction SDK** hand rigs (grab, poke, ray interactors ready to use),
- Android / Quest project settings already configured (Vulkan, Arm64, OpenXR, hands only),
- Guardian boundary grid hidden by default (passthrough shows the real room anyway).

After installation the template appears in **New Project → Games → "MR Quest 3 (Passthrough + Hands)"**.
Unreal renames `TP_MRQuest` to your project name in all classes, modules and config files.

> 🇵🇱 Pełna instrukcja po polsku: [docs/INSTRUKCJA.md](docs/INSTRUKCJA.md)

## What's inside

| Asset / class | Purpose |
|---|---|
| `ATP_MRQuestPawn` (C++) | VROrigin, HMD camera, motion controllers with ISDK hand rigs (`UIsdkHandRigComponentLeft/Right`), `LocalFloor` tracking origin, `IMC_IsdkHand` input mapping, persistent passthrough started in `BeginPlay` |
| `ATP_MRQuestGameMode` (C++) | Base game mode |
| `BP_MRPawn`, `BP_MRGameMode` | Blueprint children – extend these in your project |
| `L_MR` | Empty startup map (no sky / fog – they would cover passthrough) |

Pawn settings (Class Defaults of `BP_MRPawn`):

- `bStartPassthrough` – turn passthrough on/off,
- `PassthroughOpacity` – below 1 dims the real room,
- `TrackingOrigin` – `LocalFloor` by default (Z = 0 on the real floor).

## Requirements

| Component | Tested version |
|---|---|
| Unreal Engine | 5.6.1 (Epic Games Launcher) with **Android** target platform |
| Meta XR Plugin | 1.117.0 – installed in the engine (`Engine/Plugins/Marketplace/MetaXR`) |
| Meta XR Interaction SDK | 1.85.0 – installed in the engine (`Engine/Plugins/Marketplace/MetaXRInteraction`) |
| Android SDK / Build-Tools | API 34 / 34.0.0 |
| Android NDK | r27c (27.2.12479018) |
| JDK | OpenJDK 21 |
| IDE | Visual Studio 2022 (C++ game development) or Rider |
| Headset | Meta Quest 3 / 3S, developer mode on, hand tracking enabled |

The Meta plugins are **not** included in this repository (Meta license). Install both for UE 5.6
from Fab / Epic Games Launcher (or the Meta Horizon developer downloads) **into the engine**.

## Installation

1. Clone or download this repository.
2. Open **PowerShell as administrator** in the repository folder and run:

   ```powershell
   Set-ExecutionPolicy -Scope Process Bypass
   .\scripts\install-template.ps1
   ```

   Custom engine location: `.\scripts\install-template.ps1 -EnginePath "D:\Epic Games\UE_5.6"`
3. Restart Unreal Engine 5.6.

## Usage

1. **New Project → Games → MR Quest 3 (Passthrough + Hands)**, choose a name and location, **Create**.
2. Let Unreal build the C++ module (first open).
3. Connect the Quest via USB → **Platforms → Quest 3 → Launch** (or package an APK).
4. You should see your room and your hands. Put the controllers down to switch to hand tracking.

Before publishing, change **Project Settings → Platforms → Android → Package Name**
(the template uses `com.YourCompany.[PROJECT]`).

## Troubleshooting

| Problem | Cause / fix |
|---|---|
| Black background instead of the room | `r.PostProcessing.PropagateAlpha=True` missing in `DefaultEngine.ini`, or a Sky / Fog / Cloud actor in the level |
| No hands, flying sphere | Default GameMode is not `BP_MRGameMode` (Project Settings → Maps & Modes) |
| Double hands | An `Oculus XR Hand` component was added next to the ISDK hand rigs – remove it |
| Hands not visible | Hand tracking disabled on the headset (Settings → Movement tracking → Hand tracking) |
| Template not listed | Restart the editor; check that `UE_5.6/Templates/TP_MRQuest` exists |
| Project fails to open / build | Meta XR or Meta XR Interaction SDK not installed in the engine |
| Want the Guardian grid back | Project Settings → Meta XR → uncheck *Default Is Boundary Visibility Suppressed* |

Logs (`adb logcat -s UE`), category `LogYourProject`:
`Persistent passthrough initialized`, `Left/Right hand rig ready`, `Hand input mapping context added`.

## License

MIT – see [LICENSE](LICENSE). Meta XR Plugin and Meta XR Interaction SDK are subject to Meta's own license.
