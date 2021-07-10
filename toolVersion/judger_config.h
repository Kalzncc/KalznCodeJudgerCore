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


#define DEFAULT_LOG_PATH "judge_log.log" 
#define COMPILER_INFO_PATH "compiler.log"
#define INTERPRETER_INFO_PATH "interpreter.log"
#define FILEIO_INPUT_PATH "input.in"
#define FILEIO_OUTPUT_PATH "output.out"
#define RESULT_FILE_PATH "result.json"
#define MAX_JSON_CHAR_NUM 0x10000 // json文件的最大字符数
#define DEFAULT_UID 65534
#define MAX_TIME_LIMIT 30000
#define MAX_MEMORY_LIMIT 0x3f3f3f3f3f3f3f3f
#define MAX_STACK_LIMIT 0x7fffffff
#define MAX_OUTPUT_CHAR_BUFFER 0x3f3f3f3f
#define MAX_SPJ_TIME_LIMIT 30000
#define MAX_SPJ_MEMORY_LIMIT 0x7fffffff
#define MAX_DATA_CASE_NUMBER 1024
#define MAX_PATH_LENGTH 1024
#define MAX_CMD_STR_LENGTH 1024
#define MAX_DETAIL_LENGTH 128
#define MAX_DEBUG_INFO_LENGTH 128



enum JudgeErrorEnum {
    
    SUCCESS = 0, // 成功评测

    // 主进程产生错误
    INVALID_JUDGE_CONFIG = -100, // 评测配置不合法
    WAIT_BOX_FAILED = -101, // 无法收集box进程资源统计，
    WAIT_KILLER_FAILED= -102, // 无法wait killer进程终止
    FORK_BOX_FAILED = -103, // 无法fork box进程
    FORK_KILLER_FAILED = -104, //无法fork killer进程
    FORK_SPJ_FAILED = -105, //无法fork spj进程
    FORK_KILLER_FOR_SPJ_FAILED = -106, //无法fork用于监控spj的killer进程
    WAIT_SPJ_FAILED = -107, // 无法收集spj进程资源统计
    WAIT_KILLER_FOR_SPJ_FAILED= -108, // 无法wait用于监控spj的killer进程终止
    COMPILER_RUN_FAILED = -109, //编译器无法启动
    PERMISSION_ERROR = -110, //judge进程不是root
    ACCESS_WORKSPACE_FAILED = -111, 
    FILE_IO_INIT_FAILED = -112,
    WRITE_RESULT_FILE_FAILED = -113 ,

    // box 进程产生错误
    BOX_SECURITY_CONFIG_LOAD_FAILED = -200, // 安全配置加载失败
    BOX_DATA_REDIRECT_FAILED = -201, //评测数据重定向失败
    BOX_EXE_RUN_FAILED = -202, // 无法启动待测程序
    BOX_SET_UID_FATLED = -203, // 无法设置用户及用户组
    BOX_SET_LIMIT_FAILED = -204, // 无法设置资源分配

    // killer 进程产生错误
    KILLER_SLEEP_FAILED = -300, // 无法阻塞
    KILLER_KILL_FAILED = -301, // 无法kill掉目标进程

    // matcher 产生错误
    MATCHER_OPEN_DATA_FAILED = -400, // 无法打开文件
    MATCHER_STD_DATA_TOO_LARGE = -401, //标准答案文件字符数大于maxCharbuffer

    // log 产生错误
    LOG_IDENTIFY_ERROR_FAILED = -500, // 无法识别错误
    LOG_FILE_OPEN_FAILED = -501 // 无法打开log文件

    


};
typedef enum JudgeErrorEnum JudgeError;
 
enum JudgeResultEnum {
    // 当本次评测的JudgeError的不为SUCCESS时，JudgeResult一定是SYSTEM_ERROR。
    // 当本次评测的JudgeError为SUCCESS时，JudgeResult一定不是SYSTEM_ERROR。
    ACCEPTED = 0, // 答案正确
    WRONG_ANSWER = 1, // 答案错误
    TIME_LIMIT_EXCEEDED = 2, //时间超限 (CPU时间超限)
    MEMORY_LIMIT_EXCEEDED = 3, //内存超限
    RUNTIME_ERROR = 4, // 运行时错误
    PRESENTATION_ERROR = 5,  // 格式错误(只在严格评测模式)
    OUTPUT_LIMIT_EXCEEDED = 6,  //输出超限 (输出大小大小JudgerConfig中的maxCharBuffer)
    COMPILE_ERROR = 7, // 编译错误
    SKIP = 8, // 跳过 (仅在points mode)
    SYSTEM_ERROR = 9  //系统错误 (执行log)
};
typedef enum JudgeResultEnum JudgeResult;

enum JudgeModeEnum {
    SINGLE_RESULT_MODE = 0, // 单一结果模式，且若某样例没有AC，之后的样例直接跳过
    POINTS_MODE = 1 // 积分模式，返回每个样例的结果，即使某样例没有AC，也会接着评测后面的样例。
};
typedef enum JudgeModeEnum JudgeMode;

enum IOMode {  // 文件读写，还是标准读写
    STD_IO = 0,
    FILE_IO = 1
};
typedef enum IOMode IOMode;


enum TranslationModeEnum {
    COMPILER_MOD = 0, // 编译型语言
    INTERPRETER_MOD = 1, // 解释型语言 
    COMPILER_INTERPRETER_MOD = 2  // 半编译半解释
    /**
     * 待测代码是工作区下Main.x, x为后缀名
     * 
     * 1、compiler mode, 编译模式
     * 此模式下，judger先切换到work space目录下，执行 [compilerCmd] 指令，
     * 这里规定产生物的名字应为compilerProductName，此后judger将检测目录下是否成功生成名字为
     * compilerProductName的文件，以判定编译是否成功。如果失败则报告编译失败，否则执行runner开始评测。
     * 这里runner经初始化，会直接将进程execute到编译后的产物。
     * 
     * 2、interpreter mode 解释模式
     * 此模式下，judger先切换到work space目录下，直接开始执行runner，在runner初始化成功后，将会执行语句
     * [interpreterCmd] ,启用解释器运行代码，报错退出将按照runner的自行处理，
     * 
     * 3、compiler interpreter mode 编译解释模式
     * 此模式下，judger先切换到work space目录下，执行 [compilerCmd] 指令，
     * 这里规定产生物的名字应为compilerProductName，此后judger将检测目录下是否成功生成名字为
     * compilerProductName的文件，以判定编译是否成功。如果失败则报告编译失败，否则执行runner开始评测。
     * 在runner初始化成功后，将会执行语句 [interpreterCmd], 启用解释器运行代码，报错退出将按照runner的自行处理。
     * 
     * 
     * 不过要明确的是，此部分的配置应该由系统的部署人员完成，此部分的配置一般不会修改
     * 除非要改动OJ的编译指令，或者添加、删减某种语言。
     * 
     * 编译和解释过程中产生的输出信息会分别记录到compiler_log.txt 和 interpreter_log.txt下，
     * 它将和评测结果文件，一同交由高层的评测队列管理模块回收处理。
     * 
     * 如果出现某种语言无法适配的情况，就劳烦自行编写了。
     * 这里推荐在judger.h中声明专用的编译流程函数，在box初始化后调用。
     * 目前来说，常规的语言都可以适配。
     */
};
typedef enum TranslationModeEnum TranslationMode;

enum StrictModeEnum {
    NOT_STRICT_MODE = 0,
    STRICT_MODE = 1
    /**
     *严格评测模式：
     *如果为true，则启用严格评测模式，在严格模式下，按照以下顺序比对答案和输出：
     *1、对字符串进行变换，把答案和输出数据中的"\r\n"，用"\n"替代
     *2、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
     *3、如果输出和答案相等，则AC。
     *4、否则：将答案和输出的所有连续空白字符都用一个空格替代。
     *5、如果此时输出和答案相等，则PE。
     *6、否则WA。
     *
     *非严格模式：
     *1、对字符串进行变换，把答案和输出数据中的"\r\n"，用"\n"替代
     *2、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
     *3、将答案和输出的所有连续空白字符都用一个空格替代。
     *4、如果此时输出和答案相等，则AC。
     *5、否则WA。
     *
    */
};
typedef enum StrictModeEnum StrictMode;

struct TranslatorConfig {
    // 根据编译模式的不同，Judger对代码的处理就不同。

    TranslationMode mode; // 翻译模式，详见上述

    char* compilerPath;      // 编译器路径 绝对路径
    char** compilerOptions; // 编译指令，这里给出一个c++11编译器的指令示例：
                             // "/usr/bin/g++ -std=c++11 Main.cpp -o $compilerProductName"
                             // compilerOptions[] = {"/usr/bin/g++", "-std=c++11", "Main.cpp", "-o", "$compilerProductName", NULL};
                             // 保证最后一个有一个NULL

    char* compilerProductName;  // 编译产物名字，编译所生成的编译产物名字必须和其一致，否则Judger找不到目录下的对应文件，
                                // 会视为编译错误。
    

    char* interpreterPath;       // 解释器路径 绝对路径
    char** interpreterOptions;  // 解释器指令，这是提供给runner用解释器运行程序的指令，这里给出一个java的示例。
                                 // java $compilerProductName
                                 //  interpreterOptions[] = {"java", "$compilerProductName", NULL};
                                 // 保证最后一个有一个NULL
  
};   
typedef struct TranslatorConfig TranslatorConfig;

struct JudgeConfig {

    // 为了方便编写Compiler指令，这里待测代码均为同目录下的Main.x, 其中x为后缀名。

    // 任务ID号
    long long taskID; 

    


    //编译与评测设置
    JudgeMode judgeMode; // 评测模式
    TranslatorConfig translator; // 翻译模式


    //IO模式设置
    IOMode iOMode;


    // 严格模式设置
    StrictMode strictMode;


    //用户组设置
    uid_t uid;
    gid_t gid;


    int caseNumber;
    //路径设置
    // 输入输出文件数组，一一对应，
    char* workSpacePath; // 评测区路径 绝对路径
                         //  Judger将会在这个路径下寻找数据文件路径, 也会把生成的临时文件存放在此。
    char** inputData; //输入数据文件路径
    char** outputData; //输出数据文件路径
    char** stdAnswer; //标准答案文件路径
    // 以上三个路径是评测区相对路径

    // 资源分配
    int* maxCPUTime; //最大CPU时限
    long long* maxMemory;  //最大内存使用量
    int* maxStack; // 最大栈空间
    
    char* logPath; //log路径  绝对路径 
    

    // SPJ设置
    short isSPJ;
    char* sPJPath; // 绝对路径
    char* sPJName;


    



    
};
typedef struct JudgeConfig JudgeConfig;

struct RunConfig {
    long long taskID;
    int useCPUTime; //CPU时间消耗
    long long useMemory;//内存消耗
    int exitSignal;//结束信号量
    int exitCode;//退出代码
    JudgeError errorCode;//错误代码


    JudgeResult result;//结果
    char* resultDetail;//结果解释
};
typedef struct RunConfig RunConfig;


struct JudgerConfig {
    // 区分JudgConfig，JudgerConfig是对Judger评测机的配置，而JudgeConfig是对一次Judge任务的配置。
    // 实际上，每次任务，都必须同时传入JudgeConfig和JudgerConfig的所有信息。
    long long maxCharBuffer; // 最大字符缓存
    int maxSPJTime; // SPJ程序的最大时间
    long long maxSPJMemory;
};
typedef struct JudgerConfig JudgerConfig;


// 释放内存
void freeJudgerConfig(JudgerConfig config);
void freeJudgeConfig(JudgeConfig config);
void freeRunConfig(RunConfig config);
#endif
