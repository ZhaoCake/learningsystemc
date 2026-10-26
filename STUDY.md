# 关卡 ↔ 教程对照表

配套教程：《SystemC 入门教程（中文版）》（译自 learnnsystemc.com / Learn with Examples）。
做每关前先读对应章节（10 分钟），再动手——本仓库不重复讲概念。

| 关卡 | 主题 | 教程章节 | 关键概念 |
|------|------|----------|----------|
| ex00 | hello world | 第 1 章 Hello World | SC_MODULE / SC_METHOD / sc_start |
| ex01 | timeline | 第 7 章 Time | wait / sc_time_stamp |
| ex02 | hierarchy | 第 2 章 Module | 层次化实例化 / name() |
| ex03 | has process | 第 4 章 SC_HAS_PROCESS | 手写构造函数 |
| ex04 | binding | 第 18 章 Ports | sc_in / sc_out / 绑定 |
| ex05 | method vs thread | 第 6/9 章 | 进程语义对比 |
| ex06 | cthread | 第 10 章 CTHREAD | 时钟进程 |
| ex07 | static vs dynamic | 第 12 章 Sensitivity | 静态敏感 / notify |
| ex08 | dont initialize | 第 13 章 Initialization | 初始化执行 |
| ex09 | delta semantics | 第 11 章 Delta Cycle | 评估-更新两阶段 |
| ex10 | value change | 第 19 章 Signals | 值变化才触发 |
| ex11 | resolved | 第 22 章 Resolved Signal | 多驱动 / sc_logic |
| ex12 | time types | 第 7 章 Time | sc_time 算术 |
| ex13 | edge detect | 第 20 章 Signal Events | 边沿事件 |
| ex14 | notify | 第 9 章 Events | 立即/定时通知 |
| ex15 | wait timeout | 第 15 章 Event Queue | 超时等待 |
| ex16 | event lists | 第 15 章 | 与/或事件表 |
| ex17 | time control | 第 8 章 Simulation Stages | sc_start / sc_stop |
| ex18 | fifo blocking | 第 24 章 FIFO | 阻塞读写 |
| ex19 | fifo nonblocking | 第 24 章 | nb_read / 轮询 |
| ex20 | mutex | 第 25 章 Mutex | 互斥锁 |
| ex21 | semaphore | 第 26 章 Semaphore | 计数信号量 |
| ex22 | sc_dt | 第 5 章 Data Types | sc_uint / sc_bv |
| ex23 | report | 第 28 章 Report | 诊断分级 |
| ex24 | vcd trace | 第 29 章 Trace | 波形文件 |
| ex25 | trigger when busy | 模式篇① | 排队重入 |
| ex26 | interrupt pending | 模式篇② | 中断挂起 |
| ex27 | pipeline | 综合篇 | fifo 背压流水线 |

## 建议节奏

- **标准节奏**：每天 2-3 关，10 天通关；每关卡壳 >15 分钟看提示，看完提示自己写
- **速通节奏**：有 C++/RTL 基础可以一天 6-8 关，重点在 ex09/11/14/18/25/26（语义坑都在这）
- **做完之后**：ex27 的流水线骨架放大 = TLM 风格模型；下一步可以读教程的 TLM 部分，
  或者直接上手 Tiny Accelerator Stack（把 ex27 的 Stage 换成 MAC 阵列就是 GEMM 流水线）

## 常见卡点（来自实测）

1. **子模块必须是成员对象**——构造函数体里的局部变量会在构造结束时析构（ex02 的坑）
2. **sc_fifo 的 API 是 read/nb_read**，不是 get/put（那是 TLM 的词）
3. **手写构造函数必须 SC_HAS_PROCESS**，SC_CTOR 只是替你声明了它
4. **事件通知是脉冲不是电平**——没人等就丢，硬件思维用 pending 寄存器补救（ex26）
5. **sc_clock 相位以实测为准**——不同版本首个上升沿位置不同，写完先跑一遍对照
