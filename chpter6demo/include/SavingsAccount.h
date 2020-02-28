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

class Account;	//ǰ������

class AccountRecord {	//��Ŀ��¼
private:
	Date date;				//����
	const Account *account;	//�˻�
	double amount;			//���
	double balance;			//���
	std::string desc;		//����
public:
	//���캯��
	AccountRecord(const Date &date, const Account *account, double amount, double balance, const std::string& desc);
	void show() const;	//�����ǰ��¼
};

//���������洢��Ŀ��¼�Ķ���ӳ������
typedef std::multimap<Date, AccountRecord> RecordMap;

class Account { //�˻���
private:
	std::string m_Id;	//�ʺ�
	double m_Balance;	//���
	static double m_Total; //�����˻����ܽ��
	static RecordMap recordMap;	//��Ŀ��¼
protected:
	//����������õĹ��캯����idΪ�˻�
	Account(const Date &date, const std::string &id);
	//��¼һ���ʣ�dateΪ���ڣ�amountΪ��descΪ˵��
	void record(const Date &date, double amount, const std::string &desc);
	//���������Ϣ
	void error(const std::string &msg) const;
public:
	const std::string &GetId() const { return m_Id; }
	double GetBalance() const { return m_Balance; }
	static double GetTotal() { return m_Total; }
	//�����ֽ�dateΪ���ڣ�amountΪ��descΪ����˵��
	virtual void deposit(const Date &date, double amount, const std::string &desc) = 0;
	//ȡ���ֽ�dateΪ���ڣ�amountΪ��descΪ����˵��
	virtual void withdraw(const Date &date, double amount, const std::string &desc) = 0;
	//���㣨������Ϣ����ѵȣ���ÿ�½���һ�Σ�dateΪ��������
	virtual void settle(const Date &date) = 0;
	//��ʾ�˻���Ϣ
	virtual void Show(std::ostream &out) const;
	//��ѯָ��ʱ����
	static void query(const Date& begin, const Date& end);
};

inline std::ostream & operator << (std::ostream &out, const Account &account) {
	account.Show(out);
	return out;
}

class SavingsAccount : public Account { //�����˻���
private:
	Accumulator acc;	//����������Ϣ���ۼ���
	double rate;		//����������
public:
	//���캯��
	SavingsAccount(const Date &date, const std::string &id, double rate);
	double getRate() const { return rate; }
	//�����ֽ�
	virtual void deposit(const Date &date, double amount, const std::string &desc);
	//ȡ���ֽ�
	virtual void withdraw(const Date &date, double amount, const std::string &desc);
	//������Ϣ��ÿ��1��1�յ���һ�θú���
	virtual void settle(const Date &date);
};

class CreditAccount : public Account { //�����˻���
private:
	Accumulator acc;	//����������Ϣ���ۼ���
	double credit;		//���ö��
	double rate;		//Ƿ���������
	double fee;			//���ÿ����

	double getDebt() const {	//���Ƿ���
		double m_Balance = GetBalance();
		return (m_Balance < 0 ? m_Balance : 0);
	}
public:
	//���캯��
	CreditAccount(const Date &date, const std::string &id, double credit, double rate, double fee);
	double getCredit() const { return credit; }
	double getRate() const { return rate; }
	double getFee() const { return fee; }
	double getAvailableCredit() const {	//��ÿ�������
		if (GetBalance() < 0)
			return credit + GetBalance();
		else
			return credit;
	}
	//�����ֽ�
	virtual void deposit(const Date &date, double amount, const std::string &desc);
	//ȡ���ֽ�
	virtual void withdraw(const Date &date, double amount, const std::string &desc);
	//������Ϣ����ѣ�ÿ��1�յ���һ�θú���
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
