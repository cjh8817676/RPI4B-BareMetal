.section ".text.boot"

.global _start

_start:
    // 讀取當前處理器 ID
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, master              // 如果是 CPU0，跳到 master
    b       hang                    // 其他 CPU 進入無限迴圈

master:
    // 設定堆疊指標
    ldr     x0, =_start
    mov     sp, x0

    // 清除 BSS 段
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
    sub     x1, x1, x0
    bl      memzero

    // 跳到 C 程式碼
    bl      main
    b       hang

hang:
    wfe
    b       hang

memzero:
    str     xzr, [x0], #8
    subs    x1, x1, #8
    b.gt    memzero
    ret