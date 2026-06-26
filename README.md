# PoisonCircle（UEforNetEase）

基于 **Unreal Engine 5.7** 的第三人称动作/战斗游戏，核心玩法为"毒圈"缩圈生存系统。

## 项目概览

- **引擎版本**: Unreal Engine 5.7
- **项目名称**: PoisonCircle
- **C++ 模块**: `PoisonCircle`（Runtime）
- **目标平台**: Windows (DX12 SM6)、Linux (Vulkan SM6)、Mac (Metal SM6)
- **输入系统**: Enhanced Input
- **渲染特性**: Lumen 全局光照/反射、硬件光追、虚拟阴影贴图、Substrate 材质、MegaLights
- **仓库地址**: `https://git.code.tencent.com/Yulljc/UE.git`

## 快速开始

### 环境要求

- Unreal Engine 5.7
- Visual Studio 2022 (Windows)
- 支持 DirectX 12 SM6 / Vulkan SM6 的显卡

### 构建

```bash
# 生成项目文件
Engine/Build/BatchFiles/RunUAT.bat -project="UEforNetEase.uproject" -game -generateprojectfiles

# 编译 Editor 目标
Engine/Build/BatchFiles/RunUAT.bat BuildEditor -project="UEforNetEase.uproject" -platform=Win64 -configuration=Development

# 编译 Game 目标
Engine/Build/BatchFiles/RunUAT.bat BuildCookRun -project="UEforNetEase.uproject" -platform=Win64 -configuration=Development
```

### 打开项目

直接双击 `UEforNetEase.uproject` 在 Unreal Editor 中打开，或通过 Epic Games Launcher 打开。

## 项目结构

```
UEforNetEase/
├── UEforNetEase.uproject          # 项目文件
├── Source/
│   └── PoisonCircle/              # C++ 运行时模块
│       ├── PoisonCircle.Build.cs  # 模块构建配置
│       ├── PoisonCircleManager.h  # 毒圈管理器（核心玩法）
│       └── PoisonCircleManager.cpp
├── Config/
│   ├── DefaultEngine.ini          # 引擎配置（渲染、地图、碰撞等）
│   ├── DefaultGame.ini            # 游戏配置
│   ├── DefaultEditor.ini          # 编辑器配置
│   └── DefaultInput.ini           # 输入配置
└── Content/                       # 游戏资源（Blueprint）
    ├── Map/                       # 关卡
    │   └── Level/                 # StartMap（默认地图）、Map
    ├── 主角/                      # 主角色 & GameMode
    ├── Variant_Combat/            # 战斗系统
    │   ├── Blueprints/            # 战斗角色、敌人、AI、EQS、可交互物
    │   ├── Anims/                 # 动画蓝图、蒙太奇、Notify
    │   ├── Input/                 # 输入动作（蓄力/连击攻击、镜头切换）
    │   ├── UI/                    # UI_LifeBar 等
    │   └── VFX/                   # Niagara 粒子特效
    ├── GamePlay/                  # 游戏流程
    │   ├── BeginPlay/             # 开局流程（角色检测、关卡切换）
    │   ├── EndPlay/               # 结算流程（胜利/失败动画、GI_State）
    │   ├── Cores/                 # 核心玩法资产
    │   ├── RunTime/               # 运行时逻辑
    │   ├── SaveData/              # 存档数据
    │   └── Weather/               # 天气系统
    ├── AllAboutChest/             # 宝箱/解密 UI 系统
    ├── Found/                     # 敌人 AI 系统
    │   ├── AI/                    # 行为树（Decorators、Tasks）
    │   ├── BP/                    # 敌人蓝图（Sabertooth、Stone Golem、Stickman）
    │   └── Monsters/              # 怪物资源
    ├── Characters/                # 角色模型、动画、材质
    ├── Character/                 # 角色相关资产（血条 UI、输入、动画）
    ├── Input/                     # Enhanced Input（IMCs、Input Actions）
    ├── Audio/                     # 音频资产（结算动画音频、数字跳动音效）
    ├── Music/                     # 音乐
    ├── BattleEffects/             # 战斗特效（材质、贴图）
    ├── MWLandscapeAutoMaterial/   # 自动地形材质系统
    ├── LevelPrototyping/          # 原型搭建资源
    ├── Fantastic_Dungeon_Pack/    # Fab 商城地牢主题资产包
    ├── PN_interactiveSpruceForest/ # 交互式森林环境
    ├── ParagonAurora/             # Paragon Aurora 角色资产
    └── Developers/                # 开发者沙盒目录
```

## 核心玩法：毒圈缩圈系统

`APoisonCircleManager`（`Source/PoisonCircle/PoisonCircleManager.h:30`）是整个游戏核心机制的 C++ 实现。它是一个可 Tick 的 `AActor`，管理缩圈安全区系统。

### 工作原理

1. **开始回合** → 调用 `StartRound()`，从 `InitialRadius` / `InitialCenter` 初始化当前圈
2. **缩圈事件** → 由 `FPoisonCircleShrinkEvent` 数组驱动，每个事件定义：
   - `DelayBeforeShrink` — 缩圈前的等待时间
   - `ShrinkDuration` — 缩圈动画持续时间
   - `EndRadius` — 目标半径
   - `TargetCenter` — 目标圆心
3. **每 Tick** → `AdvanceShrink()` 按时间进度在起点和终点之间 Lerp（线性插值）半径和圆心
4. **圈外伤害** → 按 `DamageInterval` 周期对 `AffectedPawnClass` 类型的 Actor 施加 `DamagePerSecond` 伤害
5. **视觉表现** → `UpdateVisualParameters()` 将当前半径和圆心写入 `UMaterialParameterCollection`，驱动圈外变暗的后处理材质
6. **全部缩圈完成后** → `OnAllShrinkEventsComplete()` 在指定位置生成 Actor（如胜利触发器）

### 蓝图 API

| 函数 | 说明 |
|------|------|
| `StartRound()` | 开始缩圈回合 |
| `StopRound()` | 停止当前回合 |
| `IsRoundActive()` | 回合是否进行中 |
| `GetCurrentRadius()` | 获取当前安全圈半径 |
| `GetCurrentCenter()` | 获取当前安全圈圆心 |
| `IsLocationInsideSafeCircle(Location)` | 检查某位置是否在安全圈内 |
| `IsActorOutsideSafeCircle(Actor)` | 检查某 Actor 是否在安全圈外 |

### 默认缩圈配置

构造函数预置了两阶段缩圈：
1. 9000 → 4000（90 秒延迟 + 30 秒缩圈，目标圆心 1720, 3160）
2. 4000 → 2800（60 秒延迟 + 20 秒缩圈，目标圆心 0, 2000）

## 战斗系统

基于 Blueprint 和 StateTree 的完整战斗框架（`Content/Variant_Combat/`）：

- **`BP_CombatCharacter`** — 玩家战斗角色
- **`BP_CombatEnemy`** — 敌人（由 `ST_CombatEnemy` StateTree 驱动的 AI）
- **`BP_CombatAIController`** — 战斗 AI 控制器
- **`BP_Combat_EnemySpawner`** — 敌人生成器
- **攻击方式**: 蓄力攻击（`IA_ChargedAttack`）、连击攻击（`IA_ComboAttack`）、镜头侧切换
- **可交互物**: 激活物、检查点、伤害体积、训练假人、岩浆地板
- **EQS**: 环境查询系统用于 AI 空间决策

## 插件

| 插件 | 用途 |
|------|------|
| GameplayStateTree | 战斗敌人 AI 行为树 |
| BinkMedia | Bink 视频播放 |
| Landmass + Water | 地形和开放世界水体 |
| ModelingToolsEditorMode | 编辑器建模工具（仅 Editor） |

## 游戏流程

1. **`StartMap`** — 准备地图，播放开场动画后跳转到游戏地图
2. **`Map`** — 主游戏关卡，毒圈缩圈战斗
3. **结算** — 成功动画 / 失败动画，由 `GI_State` GameInstance 管理状态

## 关键配置

- **默认地图**: `StartMap`
- **默认 GameMode**: `BP_Gamemode`（`Content/主角/`）
- **GameInstance**: `GI_State`（`Content/GamePlay/EndPlay/`）
- **输入系统**: Enhanced Input（`EnhancedPlayerInput` + `EnhancedInputComponent`）

## 开发说明

### 添加 C++ 类

1. 在 `Source/PoisonCircle/` 中添加 `.h` / `.cpp`
2. 类上包含 `POISONCIRCLE_API` 宏
3. 如果新增模块依赖，修改 `PoisonCircle.Build.cs` 后重新生成项目文件

### 分支工作流

- `main` 分支 — 稳定版本
- `feature/*` 分支 — 功能开发分支
- 中文提交信息
- 通过 Pull Request 合并到 `main`

### 资产来源

- Fab 商城免费资产：`Fantastic_Dungeon_Pack`（地牢主题）、`ParagonAurora`（角色）
- 外部怪物资源：Sabertooth、Stone Golem、ROG Creatures、Stickman
- 交互式森林：`PN_interactiveSpruceForest`

### 团队

| 贡献者 | 提交数 |
|--------|--------|
| yqggg | 20 |
| Yul-ljc | 3 |
| Sin诚 | 1 |

## 许可证

暂未指定。
