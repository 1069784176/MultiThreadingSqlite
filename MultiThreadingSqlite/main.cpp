#include <QtCore/QCoreApplication>
#include <qthread.h>
#include "sqlite\sqlite3.h"
#include "qdebug.h"
#include <process.h>  
#include <qtimer.h> 
using namespace std;

#define DBPATH "D:/TBuild/缓存数据库/zsHelperCache"

#define READTIMES 200
#define WRITETIMES 200
#define READSWITCH 1
#define WRITESWITCH 1
class sqlite_data;
class read_thread_1;
class read_thread_2;
class write_thread_1;
class write_thread_2;
class write_thread_3;   

int data_result(void * data, int argc, char **argv, char ** col_name);

class sqlite_data
{
public:
	static sqlite_data * getDataEntityInstance()
	{
		if (_data_entity == NULL)
			_data_entity = new sqlite_data;
		return _data_entity;
	}

	void init_sqlite()
	{
		//sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
		std::string dbPath = QString::fromLocal8Bit(DBPATH).toStdString();
		int rert = sqlite3_open(dbPath.c_str(), &_sqlite_db);

		_wt_1 = NULL;
		_wt_2 = NULL;
		_wt_3 = NULL;   
	}

	void free_sqlite() 
	{
		sqlite3_close(_sqlite_db);
	}

	bool inser_data(int pflag,QString & remark)
	{
		string str_sql = "";
		str_sql += "insert into rw_test(remark)";
		str_sql += "values('";
		str_sql += remark.toLatin1();
		str_sql += "');";  
		char* err_msg;
		int res = sqlite3_exec(_sqlite_db, str_sql.c_str(), NULL, NULL, NULL);
		if (res != SQLITE_OK)
		{ 
			return false;
		}   
		qDebug() << QString(QStringLiteral("pflag:%1;插入数据:%2;")).arg(QString::number(pflag)).arg(remark) << endl;
		return true;
	}

	bool select_data()
	{
		char* err_msg;
		int res = sqlite3_exec(_sqlite_db, "select * from rw_test   limit 1;", data_result, 0, &err_msg); 
		if (res != SQLITE_OK)
		{ 
			return false;
		} 
		return true;
	} 

	bool delete_data(const string & remark)
	{
		string str_sql = "";
		str_sql += "delete from rw_test where remark='";
		str_sql += remark;
		str_sql += "';";

		char* err_msg;
		int res = sqlite3_exec(_sqlite_db, str_sql.c_str(), 0, 0, &err_msg);
		if (res != SQLITE_OK)
		{
			return false;
		}
		return true;
	}

	bool clear_data() 
	{
		string str_sql = "";
		str_sql += "delete from rw_test;"; 

		char* err_msg;
		int res = sqlite3_exec(_sqlite_db, str_sql.c_str(), 0, 0, &err_msg);
		if (res != SQLITE_OK)
		{
			return false;
		} 
		qDebug() << QString(QStringLiteral("======清空数据;")) << endl;
		return true;
	}
	static void freeDataEntity() {
		if (_data_entity != NULL)
		{ 
			delete _data_entity; 
		}
	}
	 
public:
	static sqlite3 * _sqlite_db;//多线程需要使用同一个 必须使用静态的 
	read_thread_1 * _rt_1;
	read_thread_2 * _rt_2;


	write_thread_1 * _wt_1;
	write_thread_2 * _wt_2;
	write_thread_3 * _wt_3;
	

private:
	static sqlite_data * _data_entity;
private:
};
sqlite_data * sqlite_data::_data_entity = NULL;
sqlite3 * sqlite_data::_sqlite_db = NULL;

int data_result(void * data, int argc, char **argv, char ** col_name)
{
	for (int i = 0; i < argc; i++)
	{
		qDebug() << col_name[i] << " = " << (argv[i] ? argv[i] : "NULL") << ", ";
		std::string remark = std::string(argv[i]);
		sqlite_data::getDataEntityInstance()->delete_data(remark);
	}
	qDebug() << endl;
	return 0;
}

class read_thread_1:public QThread
{
public:
	read_thread_1() {}
	void run() 
	{
		while (READSWITCH)
		{
			//_sleep(READTIMES);
			sqlite_data::getDataEntityInstance()->select_data(); 
		}
	}
};

class read_thread_2 :public QThread
{
public:
	read_thread_2() {}
	void run()
	{
		while (READSWITCH)
		{  
			//_sleep(READTIMES);
			sqlite_data::getDataEntityInstance()->select_data();
		}
	}
};

class write_thread_1 :public QThread
{
public:
	write_thread_1() :_index(0) {}
	void run()
	{
		while (WRITESWITCH)
		{
			//_sleep(READTIMES);
			QString print = QString("write_thread_1	_%1").arg(QString::number(_index++));
			sqlite_data::getDataEntityInstance()->inser_data(1,print);
		}
	}
private:
	int _index;
};

class write_thread_2 :public QThread
{
public:
	write_thread_2() :_index(0) {}
	void run()
	{
		while(WRITESWITCH)
		{
			//_sleep(READTIMES);
			QString print = QString("write_thread_2	_%1").arg(QString::number(_index++));
			sqlite_data::getDataEntityInstance()->inser_data(2,print);
		}
	}
private:
	int _index;
};

class write_thread_3 :public QThread
{
public:
	write_thread_3():_index(0){}
	void run()
	{
		while(WRITESWITCH)
		{
			//_sleep(READTIMES);
			QString print = QString("write_thread_3	_%1").arg(QString::number(_index++));
			sqlite_data::getDataEntityInstance()->inser_data(3,print);
		}
	}
private:
	int _index;
};

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);  
	sqlite_data::getDataEntityInstance()->init_sqlite(); 
	sqlite_data::getDataEntityInstance()->clear_data(); 
	sqlite_data::getDataEntityInstance()->_rt_1 = new read_thread_1;
	sqlite_data::getDataEntityInstance()->_rt_2 = new read_thread_2; 

	sqlite_data::getDataEntityInstance()->_wt_1 = new write_thread_1;
	sqlite_data::getDataEntityInstance()->_wt_2 = new write_thread_2;
	sqlite_data::getDataEntityInstance()->_wt_3 = new write_thread_3;



	sqlite_data::getDataEntityInstance()->_wt_1->start(); 
	sqlite_data::getDataEntityInstance()->_wt_2->start(); 
	sqlite_data::getDataEntityInstance()->_wt_3->start();

	sqlite_data::getDataEntityInstance()->_rt_1->start();
	sqlite_data::getDataEntityInstance()->_rt_2->start(); 

	//sqlite_data::freeDataEntity();
	return a.exec();
}
