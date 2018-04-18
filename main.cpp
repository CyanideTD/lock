#include "global_serv.h"
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// 0: ��ʼ״̬
// -1: stop
int g_flag = 0;

void sys_sig_kill(int signal)
{
	g_flag = -1;
}

void signal_kill(int signal)
{
	g_flag = -1;
	printf("recv kill signal[%d]\n", signal);
}


// �����û��ź�
TVOID InitSignal(){
	struct sigaction sa;
	sigset_t sset;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sys_sig_kill;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);

	 //����SIGPIPE�ź�
	signal(SIGPIPE,SIG_IGN);
	signal(SIGCHLD,SIG_IGN);

	sigemptyset(&sset);
	sigaddset(&sset, SIGSEGV);
	sigaddset(&sset, SIGBUS);
	sigaddset(&sset, SIGABRT);
	sigaddset(&sset, SIGILL);
	sigaddset(&sset, SIGCHLD);
	sigaddset(&sset, SIGFPE);
	sigprocmask(SIG_UNBLOCK, &sset, &sset);

	signal(SIGUSR1, signal_kill);
}

int main()
{
	g_flag = 0;
	InitSignal();

    HRESULT dwRcrdFlag = 0;
    dwRcrdFlag = CGlobalServ::Init("../conf/serv_info.conf","../conf/reg_info.conf");
    assert(dwRcrdFlag == S_OK);

    //�������߳�
    dwRcrdFlag = CGlobalServ::Instance()->Start();
    assert(dwRcrdFlag == S_OK);
    
	// �����ź����
	while (1)
	{
		// �յ��ź�����,����һֱ�ȴ�
		if (g_flag == -1)
		{
			TSE_LOG_INFO(CGlobalServ::g_poServLog, ("Receive stop signal!"));
			TSE_LOG_INFO(CGlobalServ::g_poServLog, ("Stop new req, wait to exit ..."));

			//�յ��ź�֮��ֹͣ����, ��ʱ���˳�
			CGlobalServ::Instance()->StopProcess();

			TSE_LOG_INFO(CGlobalServ::g_poServLog, ("Stop success!"));
			break;
		}
		sleep(1);
	}

	return 0;
}


