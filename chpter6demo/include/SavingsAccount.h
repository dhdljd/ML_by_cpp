#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

//#include <iostream>
//#include <cmath>
#include <string>
#include "date.h"
#include "accumulator.h"
#include <map>
#include <istream>
#include <stdexcept>

using namespace std;

class Account;	//前置声明

class AccountRecord {	//账目记录
private:
	Date date;				//日期
	const Account *account;	//账户
	double amount;			//金额
	double balance;			//余额
	std::string desc;		//描述
public:
	//构造函数
	AccountRecord(const Date &date, const Account *account, double amount, double balance, const std::string& desc);
	void show() const;	//输出当前记录
};

//定义用来存储账目记录的多重映射类型
typedef std::multimap<Date, AccountRecord> RecordMap;

class Account { //账户类
private:
	std::string m_Id;	//帐号
	double m_Balance;	//余额
	static double m_Total; //所有账户的总金额
	static RecordMap recordMap;	//账目记录
protected:
	//供派生类调用的构造函数，id为账户
	Account(const Date &date, const std::string &id);
	//记录一笔帐，date为日期，amount为金额，desc为说明
	void record(const Date &date, double amount, const std::string &desc);
	//报告错误信息
	void error(const std::string &msg) const;
public:
	const std::string &GetId() const { return m_Id; }
	double GetBalance() const { return m_Balance; }
	static double GetTotal() { return m_Total; }
	//存入现金，date为日期，amount为金额，desc为款项说明
	virtual void deposit(const Date &date, double amount, const std::string &desc) = 0;
	//取出现金，date为日期，amount为金额，desc为款项说明
	virtual void withdraw(const Date &date, double amount, const std::string &desc) = 0;
	//结算（计算利息、年费等），每月结算一次，date为结算日期
	virtual void settle(const Date &date) = 0;
	//显示账户信息
	virtual void Show(std::ostream &out) const;
	//查询指定时间内
	static void query(const Date& begin, const Date& end);
};

inline std::ostream & operator << (std::ostream &out, const Account &account) {
	account.Show(out);
	return out;
}

class SavingsAccount : public Account { //储蓄账户类
private:
	Accumulator acc;	//辅助计算利息的累加器
	double rate;		//存款的年利率
public:
	//构造函数
	SavingsAccount(const Date &date, const std::string &id, double rate);
	double getRate() const { return rate; }
	//存入现金
	virtual void deposit(const Date &date, double amount, const std::string &desc);
	//取出现金
	virtual void withdraw(const Date &date, double amount, const std::string &desc);
	//结算利息，每年1月1日调用一次该函数
	virtual void settle(const Date &date);
};

class CreditAccount : public Account { //信用账户类
private:
	Accumulator acc;	//辅助计算利息的累加器
	double credit;		//信用额度
	double rate;		//欠款的日利率
	double fee;			//信用卡年费

	double getDebt() const {	//获得欠款额
		double m_Balance = GetBalance();
		return (m_Balance < 0 ? m_Balance : 0);
	}
public:
	//构造函数
	CreditAccount(const Date &date, const std::string &id, double credit, double rate, double fee);
	double getCredit() const { return credit; }
	double getRate() const { return rate; }
	double getFee() const { return fee; }
	double getAvailableCredit() const {	//获得可用信用
		if (GetBalance() < 0)
			return credit + GetBalance();
		else
			return credit;
	}
	//存入现金
	virtual void deposit(const Date &date, double amount, const std::string &desc);
	//取出现金
	virtual void withdraw(const Date &date, double amount, const std::string &desc);
	//结算利息和年费，每月1日调用一次该函数
	virtual void settle(const Date &date);

	//virtual void Show() const;
	virtual void Show(std::ostream &out) const;
};

class AccountException : public std::runtime_error {
private:
	const Account *account;
public:
	AccountException(const Account *account, const std::string &msg)
		: runtime_error(msg), account(account) { }
	const Account *getAccount() const { return account; }
};

#endif // SAVINGSACCOUNT_H
