#ifndef HEADER_CONFIGDIALOG
#define HEADER_CONFIGDIALOG

#include <QtGui/QTableWidgetItem>
#include <QtGui/QListWidgetItem>
#include <QtCore/QString>
#include <QtGui/QAbstractButton>
#include <QErrorMessage>

#include "ui_ConfigDialog.h"
#include "AccountList.h"
#include "sflphone_kdeview.h"

class sflphone_kdeView;

class ConfigurationDialog : public QDialog, private Ui::ConfigurationDialog
{
	Q_OBJECT

private:
	static AccountList * accountList;
	QErrorMessage * errorWindow;
	MapStringString * codecPayloads;
	bool accountsChangedEnableWarning;
	

public:
	ConfigurationDialog(sflphone_kdeView *parent = 0);
	~ConfigurationDialog();
	static AccountList * getAccountList();

	void loadAccount(QListWidgetItem * item);
	void saveAccount(QListWidgetItem * item);

	void loadAccountList();
	void saveAccountList();
	
	void addAccountToAccountList(Account * account);

	void loadCodecs();
	void saveCodecs();

	void loadOptions();
	void saveOptions();
	
	void setPage(int page);
	
	void updateCodecListCommands();
	void updateAccountListCommands();

private slots:
	void changedAccountList();
	
	void on_toolButton_codecUp_clicked();
	void on_toolButton_codecDown_clicked();
	void on_button_accountUp_clicked();
	void on_button_accountDown_clicked();
	void on_button_accountAdd_clicked();
	void on_button_accountRemove_clicked();
	void on_edit1_alias_textChanged(const QString & text);
	void on_listWidget_accountList_currentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous );
	void on_spinBox_SIPPort_valueChanged ( int value );
	void on_buttonBoxDialog_clicked(QAbstractButton * button);
	void on_tableWidget_codecs_currentCellChanged(int currentRow);
	void on_toolButton_accountsApply_clicked();
	
	void on1_accountsChanged();
	void on1_parametersChanged();
	void on1_errorAlert(int code);
	

};

#endif 