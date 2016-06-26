# stm32f4-template
stm32f4 template using STM32 standard peripheral libraries

[TOC]

## Folder Structure
<pre>
.
├── inc
├── lib
│   ├── CMSIS
│   │   ├── inc
│   │   └── src
│   └── STM32F4xx_StdPeriph_Driver
│       ├── inc
│       └── src
└── src
</pre>

- inc: 專案的標頭檔，只有全專案共用的標頭檔才需放在此目錄
- src: 專案的程式碼，依模組再建立子目錄
- lib: 放置別人開發的程式庫，通常不需修改其內容