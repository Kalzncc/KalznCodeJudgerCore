#ifndef JUDGER_CONFIG_H
#define JUDGER_CONFIG_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <grp.h>
#include <dlfcn.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <errno.h>


#define DEFAULT_LOG_PATH                  "judge_log.log" 
#define DEFAULT_COMPILER_INFO_PATH        "compiler.log"
#define DEFAULT_INTERPRETER_INFO_PATH     "interpreter.log"
#define DEFAULT_RESULT_FILE_PATH          "result.json"
#define FILEIO_INPUT_PATH                 "input.in"
#define FILEIO_OUTPUT_PATH                "output.out"


#define MAX_JSON_CHAR_NUM                 0x10000
#define DEFAULT_UID                       65534
#define MAX_TIME_LIMIT                    60000
#define MAX_MEMORY_LIMIT                  0x3f3f3f3f
#define MAX_STACK_LIMIT                   0x7fffffff
#define MAX_OUTPUT_CHAR_BUFFER            0x3f3f3f3f
#define MAX_SPJ_TIME_LIMIT                60000
#define MAX_SPJ_MEMORY_LIMIT              0x7fffffff
#define MAX_DATA_CASE_NUMBER              1024
#define MAX_PATH_LENGTH                   1024
#define MAX_CMD_STR_LENGTH                1024
#define MAX_DETAIL_LENGTH                 128
#define MAX_DEBUG_INFO_LENGTH             128



typedef enum JudgeErrorEnum {
    
    SUCCESS = 0, /*成功评测*/

    /* 主进程产生错误*/
    INVALID_JUDGE_CONFIG = -100, /* 评测配置不合法*/
    WAIT_BOX_FAILED = -101, /* 无法收集box进程资源统计，*/
    WAIT_KILLER_FAILED= -102, /* 无法wait killer进程终止*/
    FORK_BOX_FAILED = -103, /* 无法fork box进程*/
    FORK_KILLER_FAILED = -104, /*无法fork killer进程*/
    FORK_SPJ_FAILED = -105, /*无法fork spj进程*/
    FORK_KILLER_FOR_SPJ_FAILED = -106, /*无法fork用于监控spj的killer进程*/
    WAIT_SPJ_FAILED = -107, /* 无法收集spj进程资源统计*/
    WAIT_KILLER_FOR_SPJ_FAILED= -108, /* 无法wait用于监控spj的killer进程终止*/
    COMPILER_RUN_FAILED = -109, /*编译器无法启动*/
    PERMISSION_ERROR = -110, /*judge进程不是root*/
    ACCESS_WORKSPACE_FAILED = -111,   /*无法访问工作区*/
    FILE_IO_INIT_FAILED = -112, /*文件IO模式初始化失败*/
    WRITE_RESULT_FILE_FAILED = -113 , /*结果json文件写入失败*/

    /* box 进程产生错误*/
    BOX_SECURITY_CONFIG_LOAD_FAILED = -200, /* 安全配置加载失败*/
    BOX_DATA_REDIRECT_FAILED = -201, /*评测数据重定向失败*/
    BOX_EXE_RUN_FAILED = -202, /* 无法启动待测程序*/
    BOX_SET_UID_FATLED = -203, /* 无法设置用户及用户组*/
    BOX_SET_LIMIT_FAILED = -204, /* 无法设置资源分配*/

    /* killer 进程产生错误*/
    KILLER_SLEEP_FAILED = -300, /* 无法阻塞*/
    KILLER_KILL_FAILED = -301, /* 无法kill掉目标进程*/

    /* matcher 产生错误*/
    MATCHER_OPEN_DATA_FAILED = -400, /* 无法打开文件*/
    MATCHER_STD_DATA_TOO_LARGE = -401, /*标准答案文件字符数大于maxCharbuffer*/
    MATCHER_SPJ_CAN_NOT_OPEN_DATA_FILE = -402, /*spj程序无法打开数据文件*/
    MATCHER_SPJ_ARGV_ERROR = -403,  /*spj程序参数错误*/

    /* log 产生错误*/
    LOG_IDENTIFY_ERROR_FAILED = -500, /* 无法识别错误*/
    LOG_FILE_OPEN_FAILED = -501 /* 无法打开log文件*/

    


} JudgeError;

typedef enum JudgeResultEnum {
    /* 当本次评测的JudgeError的不为SUCCESS时，JudgeResult一定是SYSTEM_ERROR。*/
    /* 当本次评测的JudgeError为SUCCESS时，JudgeResult一定不是SYSTEM_ERROR。*/
    ACCEPTED = 0, /* 答案正确*/
    WRONG_ANSWER = 1, /* 答案错误*/
    TIME_LIMIT_EXCEEDED = 2, /*时间超限 (CPU时间超限)*/
    MEMORY_LIMIT_EXCEEDED = 3, /*内存超限*/
    RUNTIME_ERROR = 4, /* 运行时错误*/
    PRESENTATION_ERROR = 5,  /* 格式错误(只在严格评测模式)*/
    OUTPUT_LIMIT_EXCEEDED = 6,  /*输出超限 (输出大小大小JudgeConfig中的maxCharBuffer)*/
    COMPILE_ERROR = 7, /* 编译错误*/
    SKIP = 8, /* 跳过 (仅在points mode)*/
    SYSTEM_ERROR = 9  /*系统错误 (执行log)*/
} JudgeResult;

typedef enum JudgeModeEnum {
    SINGLE_RESULT_MODE = 0, /* 单一结果模式，且若某样例没有AC，之后的样例直接跳过*/
    POINTS_MODE = 1, /* 积分模式，返回每个样例的结果，即使某样例没有AC，也会接着评测后面的样例。*/
    ONLY_COMPILE_MODE = 2 /* 只编译模式 （详见文档 并发评测与解释 了解详情）*/
} JudgeMode;

typedef enum IOMode {  /* 文件读写，还是标准读写*/
    STD_IO = 0,
    FILE_IO = 1
} IOMode;


typedef enum TranslationModeEnum {
    COMPILER_MODE = 0, /* 编译型语言*/
    INTERPRETER_MODE = 1, /* 解释型语言 */
    COMPILER_INTERPRETER_MODE = 2,  /* 半编译半解释*/
    DO_NOT_TANSLATE_MODE = 3 /* 不做翻译模式*/
} TranslationMode;

typedef enum StrictModeEnum {
    NOT_STRICT_MODE = 0,
    STRICT_MODE = 1
} StrictMode;

typedef struct TranslatorConfig {
    /* 根据编译模式的不同，Judger对代码的处理就不同。*/

    int mode; /* 翻译模式，详见上述*/

    char* compilerPath;      /* 编译器路径 绝对路径*/
    char** compilerOptions; /* 编译指令，这里给出一个c++11编译器的指令示例：*/
                             /* "/usr/bin/g++ -std=c++11 Main.cpp -o $compilerProductName"*/
                             /* compilerOptions[] = {"/usr/bin/g++", "-std=c++11", "Main.cpp", "-o", "$compilerProductName", NULL};*/

    char* compilerInfoPath; /* 编译器输出路径*/
    char* compilerProductName;  /* 编译产物名字，编译所生成的编译产物名字必须和其一致，否则Judger找不到目录下的对应文件，*/
                                /* 会视为编译错误。*/
    
    char* interpreterInfoPath;     /* 解释器输出路径*/
    char* interpreterPath;       /* 解释器路径 绝对路径*/
    char** interpreterOptions;  /* 解释器指令，这是提供给runner用解释器运行程序的指令，这里给出一个java的示例。*/
                                 /* java $compilerProductName*/
                                 /*  interpreterOptions[] = {"java", "$compilerProductName", NULL};*/
    int securityProcessNum; /*安全进程数*/
  
} TranslatorConfig;   

typedef struct JudgeConfig {

    
    int maxCharBuffer; /* 最大字符缓存*/

    
    char* taskID; /* 任务ID号*/


    /*编译与评测设置*/
    int judgeMode; /* 评测模式*/
    TranslatorConfig *translator; /* 翻译模式*/


    /*IO模式设置*/
    int iOMode;


    /* 严格模式设置*/
    int strictMode;


    /*用户组设置*/
    uid_t uid;
    gid_t gid;

    /*数据设置*/
    int caseNumber; /*样例个数*/
    /* 输入输出文件数组，一一对应，*/
    char* workSpacePath; /* 评测区路径 绝对路径*/
                         /*  Judger将会在这个路径下寻找数据文件路径, 也会把生成的临时文件存放在此。*/
    char* resultPath; /* result的json保存地址*/
    char** inputData; /*输入数据文件路径*/
    char** outputData; /*输出数据文件路径*/
    char** stdAnswer; /*标准答案文件路径*/
    /* 以上三个路径是评测区相对路径*/

    /* 资源分配*/
    int* maxCPUTime; /*最大CPU时限*/
    int* maxMemory;  /*最大内存使用量*/
    int* maxStack; /* 最大栈空间*/
    char* logPath; /*log路径  绝对路径 */
    

    /* SPJ设置*/
    int isSPJ; /*是否启用SPJ*/
    char* sPJPath; /* 绝对路径*/
    char* sPJName; /*SPJ程序的名字*/
    int maxSPJTime; /* SPJ程序的最大时间*/
    int maxSPJMemory; /*SPJ程序最大内存耗费*/


} JudgeConfig;

typedef struct RunConfig {
    long long taskID; /*任务ID*/
    int useCPUTime; /*CPU时间消耗*/
    long long useMemory;/*内存消耗*/
    int exitSignal;/*结束信号量*/
    int exitCode;/*退出代码*/
    JudgeError errorCode;/*错误代码*/

    int useRealTime;  /*从待测程序启动，至待测程序结束所间隔时间。此项仅用于评估judger效率。 */

    JudgeResult result;/*结果*/
    char* resultDetail;/*结果解释*/
}RunConfig;

#ifdef __DEBUG
void __debugPrintnum(const char * str, long long value);
void __debugPrints(const char * str, const char* value);
void __debugPrintJudgeConfig(JudgeConfig *config);
void __debugPrintRunConfig(RunConfig *config);
#endif

/*析构函数*/
void freeJudgeConfig(JudgeConfig *config);
void freeRunConfig(RunConfig *config);
#endif
