#include "AvitoParser.h"

AvitoParser::AvitoParser(QWidget* parent)
	: QMainWindow(parent), timer(new QTimer()), sBar(new QStatusBar()), tgObject(new TelegramJacket), timerSemafor(new QTimer())
{
	ui.setupUi(this);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon("icon.png"));

	QMenu* menu = new QMenu(this);
	QAction* restoreAction = menu->addAction("CMD open and connect");
	QAction* restoreActionHide = menu->addAction("CMD disconnect");
	QAction* quitAction = menu->addAction("Exit");

	connect(restoreAction, &QAction::triggered, this, &AvitoParser::cmdOpen);
	connect(restoreActionHide, &QAction::triggered, this, &AvitoParser::cmdClose);
	connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

	trayIcon->setContextMenu(menu);
	trayIcon->setVisible(true);

	connect(trayIcon, &QSystemTrayIcon::activated, this, &AvitoParser::iconActivated);

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
	timer->start(12000);

	connect(timerSemafor, &QTimer::timeout, this, &AvitoParser::generalFuncForTimer);

	QTimer::singleShot(7000, [=]() {

		timerSemafor->start(2000);

		});
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

	countOfTopItems = ui.treeWidget->topLevelItemCount();

	int column = ui.treeWidget->currentColumn();

	offChanger = true;

	any->setText(0, "new");
	any->setText(2, QString::number((10000 + countOfTopItems * 5000)));

	any->setBackground(0, QColor(221, 221, 221, 255));
	any->setBackground(1, QColor(245, 216, 183, 255));
	any->setBackground(2, QColor(217, 225, 187, 255));
	any->setCheckState(3, any->checkState(3));

	offChanger = false;

	poolParse.append(QSharedPointer<uniqueParseObject>::create());
	//poolParse.push_back(QSharedPointer<uniqueParseObject>(new uniqueParseObject));
	poolParse.last().data()->setParam(ui.treeWidget->topLevelItem(poolParse.length() - 1)->text(0), ui.treeWidget->topLevelItem(poolParse.length() - 1)->text(1), ui.treeWidget->topLevelItem(poolParse.length() - 1)->text(2), ui.treeWidget->topLevelItem(poolParse.length() - 1)->checkState(3));

	poolParse.last().data()->setRefMassive();

	connect(poolParse.last().data(), &uniqueParseObject::messageReceived, tgObject, &TelegramJacket::sendMessage);

	any = nullptr;
}


void AvitoParser::deleteItemInList()
{
	if (ui.treeWidget->currentItem() == nullptr) return;
	countOfTopItems = ui.treeWidget->topLevelItemCount();

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

	offChanger = true;

	any->setCheckState(3, any->checkState(3));

	if (any->text(2).toInt() < (10000 + countOfTopItems * 5000)) // ������ ���� ���-�� �������� � ���������
	{
		any->setText(2, QString::number((10000 + countOfTopItems * 5000)));
	}

	if (any->checkState(3) == Qt::Unchecked) // ������ ���� ���-�� �������� � ���������
	{
		any->setBackground(3, QColor("white"));
	}
	else
	{
		any->setBackground(3, QColor(128, 243, 150, 255));
	}

	offChanger = false;

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

		if (xmlReader.isEndElement() && !myList.isEmpty())
			myList.pop_back();

		countOfTopItems = ui.treeWidget->topLevelItemCount();
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
	countOfTopItems = ui.treeWidget->topLevelItemCount();

	poolParse.clear();

	timerSemafor->stop();

	for (int count = 0; count < countOfTopItems; count++)
	{
		if (ui.treeWidget->topLevelItem(count)->text(2).toInt() < (10000 + countOfTopItems * 5000)) // ������ ���� ���-�� �������� � ���������
		{
			ui.treeWidget->topLevelItem(count)->setText(2, QString::number((10000 + countOfTopItems * 5000)));
		}

		poolParse.append(QSharedPointer<uniqueParseObject>::create());

		poolParse.last().data()->setParam(ui.treeWidget->topLevelItem(count)->text(0), ui.treeWidget->topLevelItem(count)->text(1), ui.treeWidget->topLevelItem(count)->text(2), ui.treeWidget->topLevelItem(count)->checkState(3));

		poolParse.last().data()->setRefMassive();

		connect(poolParse.last().data(), &uniqueParseObject::messageReceived, tgObject, &TelegramJacket::sendMessage);
	}

	QTimer::singleShot(7000, [=]() {

		timerSemafor->start(2000);

		});
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


void AvitoParser::generalFuncForTimer()
{
	if (poolParse.length() <= 1)
		return;

	QList<int>remaining;

	for (auto& val : poolParse)
	{
		if (val.data()->getTimer()->remainingTime() == -1)
			remaining.append((20000 + countOfTopItems * 5000));
		else
			remaining.append(val.data()->getTimer()->remainingTime());
	}

	//qDebug() << remaining; // ��� ���������

	auto minElementItFirst = std::min_element(remaining.begin(), remaining.end());
	int indexFirst = std::distance(remaining.begin(), minElementItFirst);

	auto maxElement = std::max_element(remaining.begin(), remaining.end());
	int indexMax = std::distance(remaining.begin(), maxElement);

	int valMin = remaining[indexFirst];

	if (valMin > (5000 + (poolParse.length() * 3000))) return;

	int valMax = remaining[indexMax];

	if (remaining.length() > 2)
	{
		remaining[indexFirst] = remaining[indexMax];

		minElementItFirst = std::min_element(remaining.begin(), remaining.end());

		int indexSecond = std::distance(remaining.begin(), minElementItFirst);

		if ((remaining[indexSecond] - valMin) < 4000)
		{
			//qDebug() << "NOW THIS BOTH TIMER'S CROSS FRONTIER: " << poolParse[indexFirst].data()->temporaryName << " - " << poolParse[indexFirst].data()->getTimer()->remainingTime() << " and " << poolParse[indexSecond].data()->temporaryName << " - " << poolParse[indexSecond].data()->getTimer()->remainingTime(); // ��� ���������

			int stopedInterval = poolParse[indexFirst].data()->getTimer()->interval();

			poolParse[indexFirst].data()->getTimer()->stop();

			QTimer::singleShot(2000, [=]() {

				poolParse[indexFirst].data()->getTimer()->start(stopedInterval);

				});
		}
	}

	if (remaining.length() == 2)
	{
		if ((valMax - valMin) < 4000)
		{
			//qDebug() << "BOTH TIMER CROSS FRONTIER"; // ��� ���������

			int stopedInterval = poolParse[indexMax].data()->getTimer()->interval();

			poolParse[indexMax].data()->getTimer()->stop();

			QTimer::singleShot(2000, [=]() {

				poolParse[indexMax].data()->getTimer()->start(stopedInterval);

				});
		}
	}
}


void AvitoParser::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::ActivationReason::DoubleClick) // ��������� ������������� ������ ����� ����������� � ����� ��� � ����� ����� 24
	{
		if (!windowShow)
		{
			this->show();
			windowShow = true;
		}
		else
		{
			this->hide();
			windowShow = false;
		}
	}
}


void AvitoParser::cmdOpen()
{
	AllocConsole(); // ������� ������� � ������������ � ��� ������� �������
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout); // �������������� ����������� �����
	freopen_s(&stream, "CONOUT$", "w", stderr); // �������������� ����������� ����� ������

	printf("\nOpen console for logout\n\n");
}


void AvitoParser::cmdClose()
{
	qDebug() << "\nProgramm disconnect from console.";

	FreeConsole(); // �������� ������� �� cmd. ����� cmd ��������� ������.
}
