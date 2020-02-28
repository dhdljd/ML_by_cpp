#include "SavingsAccount.h"
#include <iostream>
#include <utility>
#include <cmath>

using namespace std;
using namespace std::rel_ops;

//double Account::m_Total = 0;

/*SavingsAccount::SavingsAccount()
{
    //ctor
}*/

//AccountRecord类的实现
AccountRecord::AccountRecord(const Date &date, const Account *account, double amount, double balance, const std::string& desc)
	: date(date), account(account), amount(amount), balance(balance), desc(desc) { }

void AccountRecord::show() const {
	//date.show();
	//cout << "\t#" << account->GetId() << "\t" << amount << "\t" << balance << "\t" << desc << endl;
	cout << date << "\t#" << account->GetId() << "\t" << amount << "\t" << balance << "\t" << desc << endl;
}


//Account类的实现
double Account::m_Total = 0;
RecordMap Account::recordMap;
Account::Account(const Date &date, const string &id)
	: m_Id(id), m_Balance(0) {
	//date.show();
	//cout << "\t#" << id << " created" << endl;
	cout << date << "\t#" << id << " created" << endl;
}

void Account::record(const Date &date, double amount, const string &desc) {
	amount = floor(amount * 100 + 0.5) / 100;	//保留小数点后两位
	m_Balance += amount;
	m_Total += amount;
	AccountRecord record(date, this, amount, m_Balance, desc);
	recordMap.insert(make_pair(date, record));
	record.show();
	//cout << "\t#" << m_Id << "\t" << amount << "\t" << m_Balance << "\t" << desc << endl;
}

void Account::Show(ostream &out) const {
	out << m_Id << "\tBalance: " << m_Balance;
}

void Account::error(const string &msg) const {
	throw AccountException(this, msg);
}

void Account::query(const Date& begin, const Date& end) {
	if (begin <= end) {
		RecordMap::iterator iter1 = recordMap.lower_bound(begin);
		RecordMap::iterator iter2 = recordMap.upper_bound(end);
		for (RecordMap::iterator iter = iter1; iter != iter2; ++iter)
			iter->second.show();
	}
}

//SavingsAccount类相关成员函数的实现
SavingsAccount::SavingsAccount(const Date &date, const string &id, double rate)
	: Account(date, id), rate(rate), acc(date, 0) { }

void SavingsAccount::deposit(const Date &date, double amount, const string &desc) {
	record(date, amount, desc);
	acc.change(date, GetBalance());
}

void SavingsAccount::withdraw(const Date &date, double amount, const string &desc) {
	if (amount > GetBalance()) {
		error("not enough money");
	} else {
		record(date, -amount, desc);
		acc.change(date, GetBalance());
	}
}

void SavingsAccount::settle(const Date &date) {
	if (date.getMonth() == 1) {	//每年的一月计算一次利息
		double interest = acc.getSum(date) * rate
			/ (date - Date(date.getYear() - 1, 1, 1));
		if (interest != 0)
			record(date, interest, "interest");
		acc.reset(date, GetBalance());
	}
}

//CreditAccount类相关成员函数的实现
CreditAccount::CreditAccount(const Date& date, const string& id, double credit, double rate, double fee)
	: Account(date, id), credit(credit), rate(rate), fee(fee), acc(date, 0) { }

void CreditAccount::deposit(const Date &date, double amount, const string &desc) {
	record(date, amount, desc);
	acc.change(date, getDebt());
}

void CreditAccount::withdraw(const Date &date, double amount, const string &desc) {
	if (amount - GetBalance() > credit) {
		error("not enough credit");
	} else {
		record(date, -amount, desc);
		acc.change(date, getDebt());
	}
}

void CreditAccount::settle(const Date &date) {
	double interest = acc.getSum(date) * rate;
	if (interest != 0)
		record(date, interest, "interest");
	if (date.getMonth() == 1)
		record(date, -fee, "annual fee");
	acc.reset(date, getDebt());
}

void CreditAccount::Show(ostream &out) const {
	Account::Show(out);
	out << "\tAvailable credit:" << getAvailableCredit();
}

