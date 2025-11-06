## 快速目标
为本仓库的 AI 编码代理提供立即可用的上下文：架构概要、关键文件、构建/调试命令、项目约定和常见代码示例。

## 大体架构（要点）
- 平台：PlatformIO + STM32Cube HAL（target: STM32F103C6）。配置见 `platformio.ini`。
- 设计：硬件抽象层（`DigitalCircuit`） + 事件驱动（`Events`） + 业务协调器（`Manager`）。
- 启动流程：`main.cpp` 调用 `HAL_Init()` → `SystemClock_Config()` → `manager->init()` → 在主循环中调用 `manager->read()` 以轮询并派发事件。

## 关键文件/目录（立即查看）
- `src/main.cpp` — 程序入口，示例：如何注册事件监听器（`mDispatcher->registerListener<GpioEvent>(...)`）和主循环模式。
- `src/Manager/Manager.hpp` — 系统协调者（初始化外设、持有 `mDispatcher`、主轮询接口 `read()`）。
- `src/Data/Data.hpp` — 全局外设句柄（如 `UART_HandleTypeDef huart1`, `IWDG_HandleTypeDef hiwdg`）、`SystemClock_Config()` 与串口/看门狗初始化函数。
- `src/Events` — 事件类型与分发器（Dispatcher）相关代码，查看如何构造/触发事件。
- `src/DigitalCircuit` — 硬件抽象（GPIO、ADC、PWM、DMA、UART 等驱动封装）。
- `src/Utils` — `Logger`、`Time` 等辅助工具；日志通过 `Data.huart1` 输出。

## 构建 / 上传 / 调试 (PlatformIO)
- 构建：在项目根目录运行 `pio run`。
- 上传（使用 ST-Link）：`pio run -t upload`。
- 在线调试（ST-Link）：`pio debug -e genericSTM32F103C6`（环境名在 `platformio.ini` 中定义）。
- 串口监视（示例波特率在 `Data.hpp` 中设为 9600）：`pio device monitor -b 9600`。

提示：如果需要开启日志或看门狗，请在 `platformio.ini` 中的 `build_flags` 添加编译宏，例如 `-D_Log` 或 `-D_Dog`。

## 项目约定与常见模式
- 语言标准：C++17（`-std=c++17` 在 `platformio.ini`）。
- 单例/全局句柄：使用 `Data` 结构体在 `src/Data/Data.hpp` 中暴露外设句柄；`Logger` 实例以 `Data.huart1` 为参数创建。
- 事件驱动：优先通过事件（如 `GpioEvent`）把硬件中断/轮询转为应用逻辑，业务逻辑以监听器形式注册到 `manager->mDispatcher`。
- 驱动放置：将硬件相关类放在 `src/DigitalCircuit`（例如 `GPIO.hpp`、`PWMChannel.hpp`）。新增外设请保持此文件夹分层。

## 具体示例（可直接参考/复用）
- 注册监听器（见 `src/main.cpp`）
  - `manager->mDispatcher->registerListener<GpioEvent>(OnOpenLedEvent);`
  - 回调示例 `void OnOpenLedEvent(GpioEvent& event)` 中可调用 `HAL_GPIO_TogglePin(...)` 或调整 PWM 占空比。
- 日志用法（`Logger`）：`LogF.logF(LogLevel::INFO, "message: %d", value);`（输出设备为 `Data.huart1`）。

## 编辑/扩展建议（对 AI 代理）
- 只做小而明确的变更：新增外设类、事件监听器或修改 `Manager::init()` 时，优先在对应目录内增加文件并更新注册代码。
- 修改平台/宏时：更改 `platformio.ini` 的 `build_flags`，并在本仓库中同时更新 README 或注释说明。
- 遵循现有风格：类封装、尽量把硬件细节封装到 `DigitalCircuit`，把业务逻辑挂到事件监听器中。

## 不要假设（重要）
- 不要修改 `SystemClock_Config()`、全局外设句柄名字或 `Data` 结构的字段名，除非需要并在同文件同步更新使用处。
- 不要改变 `manager->read()` 的运行模型（它在主循环中被期望频繁调用）——如果改为中断/RTOS，请同时更新 `README.md` 与相关初始化流程。

## 如果你需要更多信息
- 想要更具体的重构或添加新外设，请告诉我目标（例如：添加 I2C 传感器驱动、把事件改为中断触发、或把日志切换为 DMA 输出），我会基于本仓库自动生成/修改相关文件并运行基本验证。

---
请审阅这些指令并指出希望补充的区域（例如更详细的构建脚本、额外示例或风格规则）。
