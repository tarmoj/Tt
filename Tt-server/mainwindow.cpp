#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	wsServer = new WsServer(9009);

	connect(wsServer, SIGNAL(newConnection(int)), this, SLOT(setClientsCount(int)));
	connect(wsServer, SIGNAL(newMessage(QString)), ui->messagesTextEdit, SLOT(append(QString)) );
	connect(wsServer, SIGNAL(newParameters(QString)), ui->parametersTextEdit, SLOT(setText(QString)) );


	//connect(wsServer, SIGNAL(pausedChanged(bool)), ui->checkBox, SLOT(setChecked(bool))  );

	on_setOscButton_clicked();


	}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setClientsCount(int clientsCount)
{
	ui->numberOfClientsLabel->setText(QString::number(clientsCount));
}


void MainWindow::on_analyzeButton_clicked()
{
	wsServer->currentCard = ui->cardNumberSpinBox->value();
	wsServer->analyze(ui->cardNumberSpinBox->value());
}

void MainWindow::on_cardNumberSpinBox_valueChanged(int value)
{
	wsServer->currentCard = value;
	wsServer->sendToVoters("card_"+QString::number(value));
}

void MainWindow::on_setOscButton_clicked()
{
	wsServer->setOscAddress(ui->oscServerLineEdit->text(), static_cast<quint16>(ui->portSpinBox->value())   );
}

void MainWindow::on_sendOscButton_clicked()
{
	wsServer->sendMainParameters();
}

void MainWindow::on_emulateButton_clicked()
{
	wsServer->emulate();
}
