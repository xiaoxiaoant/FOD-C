#include"DebugLog.h"

#include<time.h>
#include<string>
#include<vector>

void split(const std::string &s, const std::string delim, std::vector<std::string> &ret)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    ret.clear();

    while (index != std::string::npos)
    {
        ret.push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }

    if (index - last > 0)
    {
        ret.push_back(s.substr(last, index - last));
    }
}

void write_log(const char file[], int line, const char func[], const  char* fmt, ...)
//void write_log(char*fmt, ...)
{
#if !((defined _WIN32) || (defined _WIN64))
    static pthread_mutex_t MutexSingle = PTHREAD_MUTEX_INITIALIZER;
#endif

    time_t t = time(NULL);
    char strTime[64];
    memset(strTime, 0, sizeof(strTime));
    strftime(strTime, sizeof(strTime), "[%Y-%m-%d %T]", localtime(&t));

    std::string time_fmt(strTime);
    time_fmt = time_fmt.substr(1, 13);
    time_fmt.replace(10, 1, "-");
    std::string fn = "log/mainlog" + time_fmt + ".log";

#if !((defined _WIN32) || (defined _WIN64))
    pthread_mutex_lock(&MutexSingle);
#endif

#if 1
    FILE *fp0 = fopen(fn.c_str(), "aw");
#else
    FILE *fp0 = stdout;
#endif
    if (fp0)
    {
        //std::string tmp(fmt);
        //tmp = strTime + tmp;
        //tmp += "\n";
        //
        char tmpstr[327670];

        va_list argptr;
        va_start(argptr, fmt);
        //vsprintf(tmpstr, tmp.c_str(), argptr);
        vsprintf(tmpstr, fmt, argptr);
        va_end(argptr);

        std::string str(tmpstr);
        std::vector<std::string> ret;
        split(tmpstr, "\n", ret);

        for (int i = 0; i < ret.size(); i++)
        {
            if (ret[i] != "")
            {
                fprintf(fp0, "%s[%s:%d][%s]%s\n", strTime, file, line, func, ret[i].c_str());
                //fprintf(stdout, "%s[%s:%d][%s]%s\n", strTime, file, line, func, ret[i].c_str());
            }
        }


        if (fp0 != stdout)
            fclose(fp0);
    }

#if !((defined _WIN32) || (defined _WIN64))
    pthread_mutex_unlock(&MutexSingle);
#endif
}

std::string hex_to_str(const char *hex, int len)
{
    std::string ret;
    char tmpstr[20];
    ret += "\n";

    for (int i = 0; i < len; i++)
    {
        sprintf(tmpstr, "%02hhX", hex[i]);
        ret += tmpstr;
        if (i % 16 == 15)
            ret += "\n";
        else if (i % 8 == 7)
            ret += " ";
    }
    return ret;
}
