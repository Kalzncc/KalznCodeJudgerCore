# Kalzn_Code_Judger
## 简介
这是一个使用c编写的online judge的评测机，将在完善优化后作为KalznOJ的评测沙盒。Judger每次从一个json文件中读入任务的配置信息。然后生成评测结果，在测试例中，结果将写入工作区目录下的result.txt文本文件中。在正式使用时可以将结果写入json。
## 支持
目前Judger仅在centOS7以及ubnutu20.04中测试过，理论上适用于所有支持seccomp沙盒机制的linux版本在进行编译前，请确保已经按照了seccomp库。

seccomp库安装方法（以ubuntu为例）

```sh
sudo apt install libseccomp-dev libseccomp2 seccomp
```

## 编译代码
请连接seccomp库进行编译。在toolVersion目录下
```sh
gcc  judger_config.h securitylib.h securitylib.c  compilerlib.h compilerlib.c  judger_config.c loglib.h loglib.c cjson/cJSON.h cjson/cJSON.c killerlib.h killerlib.c boxlib.h boxlib.c judgerlib.h judgerlib.c matcherlib.h matcherlib.c main.c -o Judger -lseccomp
```
另外如果想在屏幕上显示一些调试信息，请加入``-D__DEBUG``编译参数。编译后，可执行文件为目录下的Judger。


## 开始使用
### 开始使用
使用时，请在Judger所在目录下运行。
```sh
sudo ./Judger <json file path> [log path]
```
其中如果省略``log path``，在json解析成功之前，log目录将默认设置为Judger同目录下的``log.log``，当json解析成功后，log路径将采用json配置中的log。这里不建议省略``log path``，建议指令中``log path``与json中配置的````log path````保持一致。
Judger评测完成后，会在工作区目录下生成一个result.txt文件。
 
JSON配置和选项说明见Wiki
