# 07 RTOS 任务规范

[← 返回主索引](../STM32工程编码规范.md)

---

- 任务函数原型：`void vXxxTask(void)`。
- 任务结构固定为 **初始化 + 无限循环 + 周期延时**：

```c
void vLedTask(void)
{
    vLedInitAll();
    while (1)
    {
        vLedExcuteAll();
        vTaskDelay(1000);
    }
}
```

- 初始化动作（`vXxxInitAll`）在进入 `while(1)` 前完成。
- 周期动作聚合为单个 `vXxxExcuteAll` 接口，任务体保持简洁。
- 延时统一使用 `vTaskDelay()`，禁止使用阻塞式空循环。

---

[← 返回主索引](../STM32工程编码规范.md)
