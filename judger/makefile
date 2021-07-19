allfile = judger_config.h securitylib.h securitylib.c  compilerlib.h compilerlib.c \
			judger_config.c loglib.h loglib.c cjson/cJSON.h cjson/cJSON.c \
			killerlib.h killerlib.c boxlib.h boxlib.c judgerlib.h judgerlib.c \
			matcherlib.h matcherlib.c main.c

kalznjudger : $(allfile)
	gcc  -D__DEBUG  -ansi -std=c89 $(allfile) -o kalznjudger -lseccomp
	sudo mv kalznjudger /usr/bin/