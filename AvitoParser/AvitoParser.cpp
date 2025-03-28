#include "AvitoParser.h"

AvitoParser::AvitoParser(QWidget* parent)
	: QMainWindow(parent), timer(new QTimer()), tgObject(new TelegramJacket), sBar(new QStatusBar())
{
	ui.setupUi(this);

	connect(ui.pushButtonAdd, &QPushButton::clicked, this, &AvitoParser::addItemInList);
	connect(ui.pushButtonAddMinus, &QPushButton::clicked, this, &AvitoParser::deleteItemInList);


	connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &AvitoParser::setData);
	connect(ui.treeWidget, &QTreeWidget::itemChanged, this, &AvitoParser::closeEditor);
	connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &AvitoParser::otherItemWasChecked);


	connect(ui.pushButtonExport, &QPushButton::clicked, this, &AvitoParser::exportXml);
	connect(ui.pushButtonImport, &QPushButton::clicked, this, &AvitoParser::importXml);
	connect(ui.pushButtonRefresh, &QPushButton::clicked, this, &AvitoParser::initializationPoolFunc);

	connect(ui.pushButtonMassiveRef, &QPushButton::clicked, this, &AvitoParser::saveRefMassive);


	QMainWindow::setStatusBar(sBar);

	startingImportXml();
	initializationPoolFunc();

	connect(timer, &QTimer::timeout, tgObject, &TelegramJacket::getUpdates);
	timer->start(12000); // ��������� ������ 5 ������
}


AvitoParser::~AvitoParser()
{}


void AvitoParser::addItemInList()
{
	QTreeWidgetItem* any = nullptr;

	if (ui.treeWidget->currentItem() == nullptr)
	{
		any = new QTreeWidgetItem();
		ui.treeWidget->addTopLevelItem(any);
	}
	else
		return;

	int column = ui.treeWidget->currentColumn();

	offChanger = true;

	any->setText(0, "new");

	any->setBackground(0, QColor(221, 221, 221, 255));
	any->setBackground(1, QColor(245, 216, 183, 255));
	any->setBackground(2, QColor(217, 225, 187, 255));

	offChanger = false;

	poolParse.push_back(QSharedPointer<uniqueParseObject>::create());
	//poolParse.push_back(QSharedPointer<uniqueParseObject>(new uniqueParseObject));
	poolParse.last()->setParam(ui.treeWidget->topLevelItem(poolParse.length() - 1)->text(0), ui.treeWidget->topLevelItem(poolParse.length() - 1)->text(1), ui.treeWidget->topLevelItem(poolParse.length() - 1)->text(2), ui.treeWidget->topLevelItem(poolParse.length() - 1)->checkState(3));
	QObject::connect(poolParse.last().data(), SIGNAL(messageReceived(QString)), tgObject, SLOT(sendMessage(QString))); // ������� ����� ������ ���������� ��� �������� �

	any = nullptr;
}


void AvitoParser::deleteItemInList()
{
	if (ui.treeWidget->currentItem() == nullptr) return;

	poolParse.removeAt(ui.treeWidget->indexOfTopLevelItem(ui.treeWidget->currentItem()));
	ui.treeWidget->takeTopLevelItem(ui.treeWidget->indexOfTopLevelItem(ui.treeWidget->currentItem()));
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

		if (any->text(2).toInt() < 20000) // ������ ���� ���-�� �������� � ���������
		{
			any->setText(2, "10000");
		}

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
	//qDebug() << "Checked " << any->text(column) << ui.treeWidget->indexOfTopLevelItem(ui.treeWidget->currentItem()) << "Size PoolParse " << poolParse.length();/////////////////////////

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

	initializationPoolFunc();

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


void AvitoParser::mousePressEvent(QMouseEvent* event) 
{
	if (event->button() == Qt::LeftButton) {
		ui.treeWidget->setCurrentItem(ui.treeWidget->invisibleRootItem());
	}
}

void AvitoParser::initializationPoolFunc()
{
	int countOfTopItems = ui.treeWidget->topLevelItemCount();

	poolParse.clear();

	for (int count = 0; count < countOfTopItems; count++)
	{

		auto newParser = QSharedPointer<uniqueParseObject>::create();

		connect(newParser.data(), &uniqueParseObject::messageReceived, tgObject, &TelegramJacket::sendMessage);

		newParser->setParam(ui.treeWidget->topLevelItem(count)->text(0), ui.treeWidget->topLevelItem(count)->text(1), ui.treeWidget->topLevelItem(count)->text(2), ui.treeWidget->topLevelItem(count)->checkState(3));

		newParser->setRefMassive();

		poolParse.append(newParser);

	}
}



void AvitoParser::saveRefMassive()
{
	if (ui.treeWidget->currentItem() == nullptr) return;

	QList<QString>referenceList = poolParse[ui.treeWidget->indexOfTopLevelItem(ui.treeWidget->currentItem())].data()->getRefMassive();

	QFile file(ui.treeWidget->currentItem()->text(0) + "_RefMassive.txt");

	if (!(file.open(QIODevice::ReadWrite | QIODevice::Truncate))) // Truncate - ��� ������� ����������� �����
	{
		qWarning() << "Error:" + ui.treeWidget->currentItem()->text(0) + "_RefMassive.txt";
		return;
	}

	QTextStream in(&file);

	for (auto& val : referenceList)
	{
		in << val << '\n';
	}

	file.close();
}