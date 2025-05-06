# Pro-face_TowerLight-XVGU3SWG-
Pro-face XVGU3SWG Tower Light: Multi-color display, USB/Ethernet connectivity, ideal for industrial settings, clear status indication, easy installation, durable, meets automation needs.

# Pro-face XVGU3SWG Tower Light Control Library in C

The Pro-face XVGU3SWG Tower Light Control Library, rewritten in C, offers a high-performance, lightweight solution for controlling the XVGU3SWG tower light, a multi-layer industrial indicator with USB and Ethernet connectivity. This library reimplements and enhances the functionality of the original TLUSBCom.dll and EZ_TLCom.dll, enabling seamless management of the tower light’s LED layers and buzzer in industrial automation systems. Optimized for Windows (Windows 7 and above), it uses the WinUSB framework for direct USB communication, eliminating proprietary driver dependencies and supporting both 32-bit and 64-bit architectures.

## Key Features
- **LED Control**: Precisely manage the three LED layers (bottom, middle, top), each supporting red, green, and blue colors. Set states (ON, OFF, DUTY) and patterns (constant, slow blink, fast blink) for flexible visual signaling.
- **Buzzer Management**: Control the integrated buzzer with high/low tones, adjustable volume (big, medium, small), and patterns (continuous, intermittent) for enhanced audible alerts.
- **USB Communication**: Leverage WinUSB for reliable, low-level USB communication, handling command transmission and response parsing with checksum validation for data integrity.
- **Error Handling**: Provide comprehensive error codes and multilingual error messages (English, Japanese, Traditional/Simplified Chinese) for effective diagnostics.
- **Cross-Platform Potential**: While designed for Windows, the modular C code supports potential adaptation to other platforms using libraries like libusb.

## Implementation Details
The library is organized into modular components:
- **Core Functionality (tl_core.c)**: Handles initialization, connection, and resource cleanup for robust USB device management.
- **LED Control (tl_led_control.c)**: Offers functions to set and retrieve LED states per layer, with parameter validation.
- **Buzzer Control (tl_buzzer_control.c)**: Configures buzzer settings and retrieves states, supporting diverse audio outputs.
- **Command Processing (tl_command.c)**: Constructs and validates command packets (LED, buzzer, status read) with checksums.
- **USB Communication (tl_usb_comm.c)**: Manages low-level USB operations using WinUSB APIs, supporting asynchronous I/O and device enumeration via GUID.
- **Error and Logging (tl_error.c, tl_log.c)**: Implements detailed error reporting and optional logging for debugging.

The main program (main.c) demonstrates a workflow: initialize the library, open a USB connection, clear LEDs, set specific LED states (e.g., red ON for layer 1, blue ON for layer 2, green ON for layer 3), and release resources. Commands use a packet format `[ESC][CMD][DataLen-H][DataLen-L][Parameters][Checksum][CR]`, ensuring compatibility with the XVGU3SWG protocol.

## Usage Example
```c
#include "tl_tower_light.h"
int main() {
    TL_Initialize();
    TL_OpenConnection(TL_FALSE);
    TL_ClearAllLEDs();
    TL_LEDStatus status = {TL_LED_ON, TL_LED_OFF, TL_LED_OFF, TL_LED_PATTERN_ON};
    TL_SetLED(TL_LAYER_ONE, &status); // Set layer 1 to red
    TL_Finalize();
    return 0;
}
```

## Benefits
Compared to the original .NET-based libraries, this C implementation reduces overhead and enables direct hardware access. Its modular design facilitates integration into C-based industrial applications, and the absence of .NET dependencies simplifies deployment. Robust error handling and logging ensure reliability in demanding environments.

## System Requirements
- **OS**: Windows 7 or later (32/64-bit)
- **Compiler**: Visual Studio 2019 or later (C11 standard)
- **Dependencies**: WinUSB, SetupAPI.lib, WinUsb.lib
- **Hardware**: XVGU3SWG Tower Light with USB connection

This library is ideal for developers needing a reliable, efficient solution for tower light control in automation systems.

# Pro-face XVGU3SWG タワーライト制御ライブラリ（C言語）

Pro-face XVGU3SWG タワーライト制御ライブラリは、C言語で再実装された高効率かつ軽量なソリューションであり、USBおよびイーサネット接続を備えた多層産業用インジケータであるXVGU3SWGタワーライトを管理します。このライブラリは、元のTLUSBCom.dllおよびEZ_TLCom.dllの機能を再現し、強化し、産業オートメーション環境でのLED層およびブザーのシームレスな制御を実現します。Windows（Windows 7以降）向けに最適化されており、WinUSBフレームワークを活用して独自のドライバを不要とし、32ビットおよび64ビットアーキテクチャをサポートします。

## 主な機能
- **LED制御**: 3層（底部、中間、上部）のLEDを精密に管理し、各層で赤、緑、青の色をサポート。状態（オン、オフ、デューティ）およびパターン（常時点灯、遅い点滅、速い点滅）を設定可能。
- **ブザー管理**: 統合ブザーは高音/低音、音量（大、中、小）、およびパターン（連続、断続的）をサポートし、音声アラートを強化。
- **USB通信**: WinUSBを用いた信頼性の高い低レベルUSB通信を実現し、チェックサム検証によるコマンド送信およびレスポンス解析を処理。
- **エラー処理**: 包括的なエラーコードと多言語エラーメッセージ（英語、日本語、繁体字/簡体字中国語）を提供し、診断を容易に。
- **クロスプラットフォームの可能性**: Windows向けに設計されているが、モジュラーなCコードにより、libusbなどを用いた他プラットフォームへの適応が可能。

## 実装詳細
ライブラリはモジュラーコンポーネントで構成されています：
- **コア機能（tl_core.c）**: 初期化、接続、リソース解放を管理し、USBデバイスの堅牢な処理を保証。
- **LED制御（tl_led_control.c）**: 各層のLED状態を設定および取得する機能を提供し、パラメータ検証を行う。
- **ブザー制御（tl_buzzer_control.c）**: ブザー設定を構成し、状態を取得し、多様な音声出力をサポート。
- **コマンド処理（tl_command.c）**: LED、ブザー、状態読み取りコマンドのパケットを構築し、チェックサムで検証。
- **USB通信（tl_usb_comm.c）**: WinUSB APIを用いた低レベルUSB操作を管理し、非同期I/OおよびGUIDによるデバイス列挙をサポート。
- **エラーおよびログ（tl_error.c、tl_log.c）**: 詳細なエラー報告およびデバッグ用のログ機能を実装。

メインプログラム（main.c）はワークフローを示します：ライブラリ初期化、USB接続確立、LEDクリア、特定のLED状態設定（例：層1を赤オン、層2を青オン、層3を緑オン）、リソース解放。コマンドは`[ESC][CMD][DataLen-H][DataLen-L][Parameters][Checksum][CR]`の形式で、XVGU3SWGプロトコルと互換性があります。

## 使用例
```c
#include "tl_tower_light.h"
int main() {
    TL_Initialize();
    TL_OpenConnection(TL_FALSE);
    TL_ClearAllLEDs();
    TL_LEDStatus status = {TL_LED_ON, TL_LED_OFF, TL_LED_OFF, TL_LED_PATTERN_ON};
    TL_SetLED(TL_LAYER_ONE, &status); // 層1を赤に設定
    TL_Finalize();
    return 0;
}
```

## 利点
元の.NETベースのライブラリに比べ、このC実装はオーバーヘッドを削減し、ハードウェアへの直接アクセスを可能にします。モジュラー設計により、Cベースの産業アプリケーションへの統合が容易で、.NET依存性の排除によりデプロイが簡素化されます。堅牢なエラー処理とログ機能により、厳しい環境での信頼性が確保されます。

## システム要件
- **OS**: Windows 7以降（32/64ビット）
- **コンパイラ**: Visual Studio 2019以降（C11標準）
- **依存ライブラリ**: WinUSB、SetupAPI.lib、WinUsb.lib
- **ハードウェア**: USB接続のXVGU3SWGタワーライト

このライブラリは、オートメーションシステムにおけるタワーライト制御のための信頼性と効率性を求める開発者に最適です。

# Pro-face XVGU3SWG塔燈控制庫（C語言）

Pro-face XVGU3SWG塔燈控制庫是以C語言重新實現的高效、輕量級解決方案，用於控制XVGU3SWG塔燈，這是一款支援USB和乙太網連接的多層工業指示器。此庫重現並增強了原有TLUSBCom.dll和EZ_TLCom.dll的功能，提供工業自動化系統中對塔燈LED層和蜂鳴器的無縫控制。該庫針對Windows系統（Windows 7及以上）進行優化，採用WinUSB框架實現直接USB通信，無需專有驅動程式，支援32位元和64位元架構。

## 主要功能
- **LED控制**：精確管理三層LED（底部、中間、頂部），每層支援紅、綠、藍三色。可設定LED狀態（開、關、占空比）和模式（恆亮、慢閃、快閃），實現靈活的視覺信號。
- **蜂鳴器管理**：內建蜂鳴器支援高/低音、音量（大、中、小）和多種模式（連續、間歇），增強音頻警報效果。
- **USB通信**：基於WinUSB實現可靠的低層USB通信，處理命令傳輸和回應解析，並透過校驗和確保數據完整性。
- **錯誤處理**：提供全面的錯誤碼和多語言錯誤訊息（英文、日文、繁體/簡體中文），便於診斷和用戶友好交互。
- **跨平台潛力**：雖為Windows設計，但模組化的C程式碼支援使用libusb等庫適配其他平台。

## 實現細節
該庫由模組化組件構成：
- **核心功能（tl_core.c）**：負責初始化、連接和資源清理，確保穩健的USB設備管理。
- **LED控制（tl_led_control.c）**：提供設定和獲取每層LED狀態的功能，並驗證參數。
- **蜂鳴器控制（tl_buzzer_control.c）**：配置蜂鳴器設定並獲取狀態，支援多樣化的音頻輸出。
- **命令處理（tl_command.c）**：構建並驗證命令數據包（LED、蜂鳴器、狀態讀取），包含校驗和。
- **USB通信（tl_usb_comm.c）**：使用WinUSB API管理低層USB操作，支援非同步I/O和透過GUID進行設備列舉。
- **錯誤與日誌（tl_error.c、tl_log.c）**：實現詳細的錯誤報告和可選的日誌功能以便除錯。

主程式（main.c）展示典型工作流程：初始化庫、開啟USB連接、清除LED、設定特定LED狀態（例如，第1層紅色開、第2層藍色開、第3層綠色開）並釋放資源。命令採用`[ESC][CMD][DataLen-H][DataLen-L][Parameters][Checksum][CR]`的數據包格式，與XVGU3SWG協議相容。

## 使用範例
```c
#include "tl_tower_light.h"
int main() {
    TL_Initialize();
    TL_OpenConnection(TL_FALSE);
    TL_ClearAllLEDs();
    TL_LEDStatus status = {TL_LED_ON, TL_LED_OFF, TL_LED_OFF, TL_LED_PATTERN_ON};
    TL_SetLED(TL_LAYER_ONE, &status); // 將第1層設為紅色
    TL_Finalize();
    return 0;
}
```

## 優勢
與原有的.NET庫相比，此C實現減少了開銷並實現了直接硬體存取。其模組化設計便於整合到基於C的工業應用中，無.NET依賴簡化了部署。穩健的錯誤處理和日誌功能確保在苛刻環境中的可靠性。

## 系統需求
- **作業系統**：Windows 7或更高版本（32/64位元）
- **編譯器**：Visual Studio 2019或更高版本（C11標準）
- **依賴庫**：WinUSB、SetupAPI.lib、WinUsb.lib
- **硬體**：具USB連接的XVGU3SWG塔燈

此庫適合尋求自動化系統中塔燈控制的可靠、高效解決方案的開發者。

