#include "AvitoParser.h"

AvitoParser::AvitoParser(QWidget* parent)
	: QMainWindow(parent), timer(new QTimer())
{
	ui.setupUi(this);

	connect(ui.pushButtonAdd, &QPushButton::clicked, this, &AvitoParser::addItemInList);
	connect(ui.pushButtonAddMinus, &QPushButton::clicked, this, &AvitoParser::deleteItemInList);

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(setData()));
	connect(ui.treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(closeEditor(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(otherItemWasChecked(QTreeWidgetItem*)));

	connect(ui.pushButtonExport, &QPushButton::clicked, this, &AvitoParser::exportXml);
	connect(ui.pushButtonImport, &QPushButton::clicked, this, &AvitoParser::importXml);
	connect(ui.pushButtonRefresh, &QPushButton::clicked, this, &AvitoParser::initializationPoolFunc);


	middleColumn = 0;
	sBar = new QStatusBar();
	QMainWindow::setStatusBar(sBar);

	startingImportXml();

	initializationPoolFunc();

	connect(timer, &QTimer::timeout, this, &AvitoParser::getUpdates);

	timer->start(5000); // ��������� ������ 5 ������

}

AvitoParser::~AvitoParser()
{}


void AvitoParser::addItemInList()
{
	QTreeWidgetItem* any = nullptr;

	if (ui.treeWidget->currentItem() == nullptr)
		any = new QTreeWidgetItem(ui.treeWidget);

	else
		return;

	int column = ui.treeWidget->currentColumn();

	offChanger = true;

	any->setText(0, "new");

	any->setBackground(0, QColor(221, 221, 221, 255));
	any->setBackground(1, QColor(245, 216, 183, 255));
	any->setBackground(2, QColor(217, 225, 187, 255));

	offChanger = false;
}


void AvitoParser::deleteItemInList()
{
	if (ui.treeWidget->currentItem() == nullptr) return;

	QTreeWidgetItem* taked = ui.treeWidget->currentItem();

	QTreeWidgetItem* parent = taked->parent();

	if (taked->parent() == nullptr)
	{
		ui.treeWidget->takeTopLevelItem(ui.treeWidget->indexOfTopLevelItem(taked));
		poolParse.removeAt(ui.treeWidget->indexOfTopLevelItem(ui.treeWidget->currentItem()));
	}
	else
		parent->takeChild(parent->indexOfChild(taked));
}


void AvitoParser::setData() // � ������ �������� ����� � ������ ��������� ��������
{
	QTreeWidgetItem* any = ui.treeWidget->currentItem(); // ����������� ��������� ��������� ������
	int column = ui.treeWidget->currentColumn(); // ����������� ���������� ����� �������� ������� (������ ���������� � 0-���)

	if (column == 3) return; // �� ��� ������������� ������ �������� �������            

	//qDebug() << "OPEN EDITOR";

	middleColumn = column;
	middleItem = any;

	ui.treeWidget->openPersistentEditor(any, column); // ��������� �������������� ������
	ui.treeWidget->editItem(any, column); // ��������� ��������
}


void AvitoParser::closeEditor(QTreeWidgetItem* any) // ���� �������� ��������� � ������ �������� ��������� (����� ������� Enter)
{
	if (offChanger) return; // ������������ ������������� ���������� ���� ������� ��� ��������� ������

	QString temporary = any->text(2).trimmed(); // ������� �������
	any->setText(2, temporary);

	if (any->text(2) != "") // ������ ���� ���-�� �������� � ���������
	{
		offChanger = true;

		any->setBackground(3, QColor(128, 243, 150, 255));
		any->setCheckState(3, any->checkState(3));

		offChanger = false;
	}
	else
	{
		offChanger = true;

		any->setBackground(3, QColor("white"));
		any->setData(3, Qt::CheckStateRole, QVariant());
		any->setText(3, "");

		offChanger = false;
	}
	ui.treeWidget->closePersistentEditor(middleItem, middleColumn); // ��������� ��������
}


void AvitoParser::otherItemWasChecked(QTreeWidgetItem* any) // ��������� �������� �������� � ������ ������������ �� ������ �������
{
	if (offChanger) return;

	int column = ui.treeWidget->currentColumn();
	//qDebug() << "Checked " << any->text(column) << ui.treeWidget->indexOfTopLevelItem(ui.treeWidget->currentItem());/////////////////////////

	if (any == middleItem && column == middleColumn)
		return;

	QString temporary = any->text(2).trimmed();
	any->setText(2, temporary);

	ui.treeWidget->closePersistentEditor(middleItem, middleColumn);
	middleItem = nullptr;
}


void AvitoParser::exportXml()
{
	QString savedFile = QFileDialog::getSaveFileName(0, "Save XML", "", "*.xml");
	QFile file(savedFile);
	file.open(QIODevice::WriteOnly);

	QXmlStreamWriter xmlWriter(&file); // �������������� ������ QXmlStreamWriter ������� �� ������ � ������� ����� ��������

	xmlWriter.setDevice(&file);
	xmlWriter.setAutoFormatting(true); // ���������� ��� ��������������� �������� �� ����� ������
	xmlWriter.setAutoFormattingIndent(2); // ����� ���������� �������� � ������� (�� ��������� 4)
	xmlWriter.writeStartDocument(); // ����� � ����� ��������� ���������

	int countOfTopItems = ui.treeWidget->topLevelItemCount();

	xmlWriter.writeStartElement("Root");

	if (countOfTopItems)
	{
		for (int val = 0; val < countOfTopItems; val++)
		{
			QTreeWidgetItem* any = ui.treeWidget->topLevelItem(val);

			recursionXmlWriter(any, xmlWriter);
		}
	}

	xmlWriter.writeEndElement();

	xmlWriter.writeEndDocument();

	file.close();
}


void AvitoParser::recursionXmlWriter(QTreeWidgetItem* some, QXmlStreamWriter& someXmlWriter)
{
	if (some->childCount())
	{
		someXmlWriter.writeStartElement("Point"); // ��������� ��������� ������� "��������" xml

		someXmlWriter.writeAttribute("Name", some->text(0)); // ����������� �������� ������ ��������� ������� ��������  

		someXmlWriter.writeAttribute("Reference", some->text(1));

		someXmlWriter.writeAttribute("Update", some->text(2));

		if (some->text(2) != nullptr)
		{
			if (some->checkState(3) == Qt::Unchecked)
				someXmlWriter.writeAttribute("Active", "0");
			else
				someXmlWriter.writeAttribute("Active", "1");
		}

		int count = some->childCount();

		for (int x = 0; x < count; x++)
		{
			recursionXmlWriter(some->child(x), someXmlWriter);
		}

		someXmlWriter.writeEndElement();
	}
	else
	{
		someXmlWriter.writeStartElement("Point"); // ��������� ��������� ������� "��������" xml  

		someXmlWriter.writeAttribute("Name", some->text(0)); // ����������� �������� ������ ��������� ������� ��������   

		someXmlWriter.writeAttribute("Reference", some->text(1)); //

		someXmlWriter.writeAttribute("Update", some->text(2));

		if (some->text(2) != nullptr)
		{
			if (some->checkState(3) == Qt::Unchecked)
				someXmlWriter.writeAttribute("Active", "0");
			else
				someXmlWriter.writeAttribute("Active", "1");
		}

		someXmlWriter.writeEndElement();

		return;
	}
}


void AvitoParser::importXml()
{
	/* ��������� ���� ��� ������ � ������� ����, ���������� � lineEditWrite */

	QString addFileDonor = QFileDialog::getOpenFileName(0, "Choose XML for import", "", "*.xml");

	if (addFileDonor == "")
	{
		return;
	}

	QFile file(addFileDonor);
	QXmlStreamReader xmlReader(&file);

	file.open(QFile::ReadWrite);

	loopXmlReader(xmlReader);

	file.close();

	QFile txtFile("tree.txt");

	if (!(txtFile.open(QIODevice::WriteOnly | QIODevice::Truncate))) // Truncate - ��� ������� ����������� �����
	{
		// qDebug() << "Don't find browse file. Add a directory with a tree.";
		sBar->showMessage("Don't find browse file. Add a directory with a tree.", 10000);
		return;
	}

	QTextStream in(&txtFile);

	in << addFileDonor << Qt::endl;

	if (addFileDonor == "")
		txtFile.remove();

	txtFile.close();
}


void AvitoParser::loopXmlReader(QXmlStreamReader& xmlReader)
{
	QList <QTreeWidgetItem*> myList;

	QTreeWidgetItem* some = nullptr;;

	ui.treeWidget->clear(); // ������� ������ ����� ��������� ����� ������

	while (!xmlReader.atEnd())
	{
		if (xmlReader.readNextStartElement())
		{
			if (xmlReader.name().toString() == "Root")
				continue;

			if (myList.length() == 0)
			{
				some = new QTreeWidgetItem(ui.treeWidget);
			}
			else
				some = new QTreeWidgetItem(myList.constLast());

			myList.push_back(some);

			offChanger = true;

			some->setBackground(0, QColor(221, 221, 221, 255));
			some->setBackground(1, QColor(245, 216, 183, 255));
			some->setBackground(2, QColor(217, 225, 187, 255));

			some->setText(0, xmlReader.name().toString());

			for (QXmlStreamAttribute& val : xmlReader.attributes())
			{
				if (val.name().toString() == "Name") some->setText(0, val.value().toString());

				if (val.name().toString() == "Reference") some->setText(1, val.value().toString());

				if (val.name().toString() == "Update") some->setText(2, val.value().toString());

				if (val.name().toString() == "Active")
				{
					if (val.value().toString() == "1")
						some->setCheckState(3, Qt::Checked);
					else
						some->setCheckState(3, Qt::Unchecked);
				}
			}

			offChanger = false;

			AvitoParser::closeEditor(some);
		}

		if (xmlReader.isEndElement())
			myList.pop_back();
	}
}


void AvitoParser::startingImportXml()
{
	/* ��������� ���� ��� ������ � ������� ����, ���������� � lineEditWrite */

	QFile file("tree.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		sBar->showMessage("Don't find browse file. Add a directory with a tree.", 10000);
		return;
	}

	QTextStream out(&file);

	QString myLine = out.readLine(); // ����� readLine() ��������� ���� ������ �� ������

	if (myLine == "")
	{
		sBar->showMessage("Don't find browse file. Add a directory with a tree.", 10000);
		return;
	}

	file.close();

	QFile xmlFile(myLine);

	if (xmlFile.exists())
	{
		xmlFile.open(QFile::ReadWrite);
	}
	else
	{
		sBar->showMessage("Don't find browse file. Add a directory with a tree.", 10000);
		return;
	}

	QXmlStreamReader xmlReader(&xmlFile);

	loopXmlReader(xmlReader);

	xmlFile.close();
}


void AvitoParser::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {

		ui.treeWidget->setCurrentItem(ui.treeWidget->invisibleRootItem());
	}
}

void AvitoParser::initializationPoolFunc()
{
	int countOfTopItems = ui.treeWidget->topLevelItemCount();

	for (int count = 0; count < countOfTopItems; count++)
	{
		poolParse.push_back(QSharedPointer<uniqueParseObject>(new uniqueParseObject));

		poolParse[count]->setParam(ui.treeWidget->topLevelItem(count)->text(0), ui.treeWidget->topLevelItem(count)->text(1), ui.treeWidget->topLevelItem(count)->text(2), ui.treeWidget->topLevelItem(count)->checkState(3));
	}
}


void AvitoParser::getUpdates()
{
	QNetworkAccessManager* manager = new QNetworkAccessManager();

	// �������� ������� ��� LongPoll (��� 0 ShortPoll) � ��������. ����� �������� offset ��� ������������� ��������� ���������� � Telegram (����� �������� �� �������� ������ ���������)
	QString urlString = QString("https://api.telegram.org/bot%1/getUpdates?offset=%2?timeout=5")
		.arg(token)
		.arg(iD);

	//QUrl url(urlString);
	//QNetworkRequest request(url);


	QNetworkRequest request(urlString);
	QNetworkReply* reply = manager->get(request);

	QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
		if (reply->error() == QNetworkReply::NoError)
		{
			//qDebug() << reply->readAll();
			//QByteArray responseData = reply->readAll();
			//QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
			QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());

			if (jsonDoc["ok"].toBool())
			{
				QJsonArray updates = jsonDoc["result"].toArray();

				for (const QJsonValue& value : updates)
				{
					//QJsonObject updateObj = value.toObject();

					//if (updateObj.contains("message")) 
					//{
					QJsonObject messageObj = value["message"].toObject();
					QJsonObject fromObj = messageObj["from"].toObject();

					QString text = messageObj["text"].toString();

					qDebug() << "Received message (" << value["update_id"].toInteger() << "): " << messageObj["text"].toString() << "chatId: " << fromObj["id"].toInteger();

					iD = value["update_id"].toInteger() + 1;

					//qDebug() << "test:" << value["update_id"].toInteger();

					//}
				}
			}
		}
		else
		{
			qDebug() << "Error:" << reply->error() << reply->errorString();
		}

		reply->deleteLater();

		});
}