
wf06.exe �@ run �_�ӴN�|�۰ʱa�i���Ҧ�����O

    > d:\My Dropbox\learnings\Forth\Ksanavm\WF06\f-source>wf06
    > WF06 (Win32) by  H.C. Chen 2010/06/10 12:44:59
    > Based on yap's KsanaVM forth machine http://tutor.ksana.tw
    > http://mfgkm-wks.wistron.com/RD/Software/Wistron EFI Forth Release Notes.aspx
    >
    > running autoexec.f        <===== �� F Language �@�˳B�z command line ����O�C argv[1] �O�{�ɱa�i�Ӫ� script �ɡAargv[2] argv[3] ... �@�@��@ forth command words ����쩳�C
    > include common.f08        <===== �@�j�� forth command words ���O�o�̭��H source code ���Φ��ɤJ�{�ɰʺA�a�w�q�X�Ӫ��C Open Firmware ���믫�N�O�o�ˡC
    > include dump.f01          <===== �ܥX debug.exe �� 'd' ���O�A dump memory�C
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

�ոլ� utility.dll ���\�� (WHQ TE's utility.dll Access32.sys in the working folder)

    > ok>variable &data    <======== �H���ܥX�@���ܼƨӵ� GetBattCap ��
    >
    > ok>&data GetBattCap .d cr &data ? cr
    > 0012fa01
    > 100         <=============== Ū��F�q���e�q

�ոլ� winio32.dll ���\�� (winio32.dll winio32.sys in the working folder)

    > ok>$70 0 ob $71 ib .b cr
    > 34          <=============== Ū��F RTC �� ��ơA�hŪ�X���A�ݨ쥦�b�� BCD �X�C
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

�Q�� winio32.dll ���\��AŪŪ�� physical memory (winio32.dll winio32.sys in the working folder)

    > ok>0xffff0 peekd .d cr         <====== CPU reset ���᪺�i�J�I�A��� BIOS ROM �̭� 0x000ffff0 �B�C
    > 00e05bea                       <====== �o�ӭȩҦ��� PC ���@�ˡA�ݬݹ藍��C ��I
    >
    > ok>0xffff0 4 + peekd .d cr
    > 2f3930f0
    >
    > ok>0xffff0 8 + peekd .d cr
    > 302f3131
    >
    > ok>0xffff0 12 + peekd .d cr
    > 00fc0038

�� memory dump �Ӭݦn�F�A dump.f01 �w�q�X�Ӫ� d ���O�u��b wf06.exe �ۤv���Ŷ��̨ϥΡC
�{�ɻݭn�@�ӯ몺���O�h��~���� physical memory �h�i�� d�C �H��g�@�ӨӥΡA

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
    >     ;                   \ �o�˴N�ܥX�@�ӷs�R�O�ӤF�C
    >
    > 512 malloc              \ allocate a 512 bytes memory block from OS
    > constant &buffer        \ assign the 512 bytes memory block starting address to a constant named &buffer
    >
    > $ffff0 &buffer 16 move-physical-memory-to-logical-address
    > &buffer d               \ dump it see see

��W���o�� sample code ���� copy - past �� wf06.exe �� command promt �N���W�i�H�ݨ쵲�G�C
BIOS F000 segment ���̫� 16 �� bytes ���T�a��ܥX�ӤF�C

