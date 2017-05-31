# MultiThreadingSqliteTestDemo
sqlite3多线程测试demo(支持多线程) 测试结果看备注
备注:
sqlite3多线程测试(5个线程;三个写,两个读删);测试结果:如果用多线程编译好的sqlite库多线程操作是不用上锁的
多个线程用的是一个共同的上下文对象（只需open一次）
