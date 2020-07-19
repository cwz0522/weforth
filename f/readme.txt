
wf06.exe 一 run 起來就會自動帶進當今所有的能力

    > d:\My Dropbox\learnings\Forth\Ksanavm\WF06\f-source>wf06
    > WF06 (Win32) by  H.C. Chen 2010/06/10 12:44:59
    > Based on yap's KsanaVM forth machine http://tutor.ksana.tw
    > http://mfgkm-wks.wistron.com/RD/Software/Wistron EFI Forth Release Notes.aspx
    >
    > running autoexec.f        <===== 像 F Language 一樣處理 command line 的能力。 argv[1] 是臨時帶進來的 script 檔，argv[2] argv[3] ... 一一當作 forth command words 執行到底。
    > include common.f08        <===== 一大堆 forth command words 都是這裡面以 source code 的形式導入臨時動態地定義出來的。 Open Firmware 的精神就是這樣。
    > include dump.f01          <===== 變出 debug.exe 的 'd' 指令， dump memory。
    > include help.f01          <===== help command
    > include winio.f01         <===== define Winio.dll all commands
    > include utility.f01       <===== define utility.dll all commands
    >
    > Utility DLL 1.19 Build date : Jan 20 2010
    > Info : Call INT 15h to read/write ECRAM
    > TE Access Version 1.04  Build date : 2009/12/15
    >
    > autoexec.f done
    >
    > ok>

試試看 utility.dll 的功能 (WHQ TE's utility.dll Access32.sys in the working folder)

    > ok>variable &data    <======== 隨手變出一個變數來給 GetBattCap 用
    >
    > ok>&data GetBattCap .d cr &data ? cr
    > 0012fa01
    > 100         <=============== 讀到了電池容量

試試看 winio32.dll 的功能 (winio32.dll winio32.sys in the working folder)

    > ok>$70 0 ob $71 ib .b cr
    > 34          <=============== 讀到了 RTC 的 秒數，多讀幾次，看到它在跳 BCD 碼。
    >
    > ok>$70 0 ob $71 ib .b cr
    > 35
    >
    > ok>$70 0 ob $71 ib .b cr
    > 36
    >
    > ok>$70 0 ob $71 ib .b cr
    > 36
    >
    > ok>$70 0 ob $71 ib .b cr
    > 37
    >
    > ok>$70 0 ob $71 ib .b cr
    > 37
    >
    > ok>$70 0 ob $71 ib .b cr
    > 38
    >
    > ok>$70 0 ob $71 ib .b cr
    > 38
    >
    > ok>$70 0 ob $71 ib .b cr
    > 39
    >
    > ok>$70 0 ob $71 ib .b cr
    > 40

利用 winio32.dll 的功能，讀讀看 physical memory (winio32.dll winio32.sys in the working folder)

    > ok>0xffff0 peekd .d cr         <====== CPU reset 之後的進入點，位於 BIOS ROM 裡面 0x000ffff0 處。
    > 00e05bea                       <====== 這個值所有的 PC 都一樣，看看對不對。 對！
    >
    > ok>0xffff0 4 + peekd .d cr
    > 2f3930f0
    >
    > ok>0xffff0 8 + peekd .d cr
    > 302f3131
    >
    > ok>0xffff0 12 + peekd .d cr
    > 00fc0038

用 memory dump 來看好了， dump.f01 定義出來的 d 指令只能在 wf06.exe 自己的空間裡使用。
臨時需要一個般的指令去把外面的 physical memory 搬進來 d。 隨手寫一個來用，

    > : move-physical-memory-to-logical-address  ( from-physical to-logical length -- )  \ max 43 characters command word length
    >                         \ phy logi len
    >     4 /                 \ phy logi len     translate length of bytes to dwords
    >     0 do
    >                         \ phy logi
    >         over i cells +
    >         over i cells +  \ phy logi phy[i] logi[i]
    >         swap peekd      \ phy logi logi[i] (phy[i])
    >         swap !          \ phy logi
    >     loop
    >     drop drop
    >     ;                   \ 這樣就變出一個新命令來了。
    >
    > 512 malloc              \ allocate a 512 bytes memory block from OS
    > constant &buffer        \ assign the 512 bytes memory block starting address to a constant named &buffer
    >
    > $ffff0 &buffer 16 move-physical-memory-to-logical-address
    > &buffer d               \ dump it see see

把上面這片 sample code 直接 copy - past 給 wf06.exe 的 command promt 就馬上可以看到結果。
BIOS F000 segment 的最後 16 個 bytes 正確地顯示出來了。

