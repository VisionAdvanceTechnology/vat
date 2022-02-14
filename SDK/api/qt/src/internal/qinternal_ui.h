#ifndef QINTERNAL_UI_H
#define QINTERNAL_UI_H

#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QEvent>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardItemModel>
#include "qinternal_core.h"

class qui_editor_unsupport : public QLabel
{
	Q_OBJECT
public:
	explicit qui_editor_unsupport(QWidget* parent = nullptr);
};

class qui_editor_combobox : public QComboBox
{
	Q_OBJECT
public:
	explicit qui_editor_combobox(qi_property* prop, QWidget* parent = nullptr);
	~qui_editor_combobox();
	
	uint32_t getValue();

protected:
	qi_property* m_prop;
	QStandardItemModel* m_model;

	void refreshItems();
	bool eventFilter(QObject* _object, QEvent* _event) override;
};

class qui_editor_uintedit : public QSpinBox
{
	Q_OBJECT
public:
	explicit qui_editor_uintedit(qi_property* prop, QWidget* parent = nullptr);

protected:
	QRegularExpressionValidator m_validator;
	QString m_format;
	qi_property* m_prop;
	int32_t m_ishex;

	QValidator::State validate(QString& text, int& pos)const override;
	int valueFromText(const QString& text)const override;
	QString textFromValue(int value)const override;

	void refreshItem();
};

class qui_editor_string : public QLineEdit
{
	Q_OBJECT
public:
	explicit qui_editor_string(qi_property_string_base* prop, QWidget* parent = nullptr);
	~qui_editor_string();

protected:
	qi_property_string_base* m_prop;

protected Q_SLOTS:
	void recvTextFinish();
};

class qui_editor_path : public QWidget
{
	Q_OBJECT
public:
	explicit qui_editor_path(qi_property* prop,qtv_property_pathmode mode, QWidget* parent = nullptr);

	QString& getResult() { return m_szvalue; }

protected:
	qi_property* m_prop;
	qtv_property_pathmode m_mode;
	QString m_szvalue;
	QLineEdit* m_txturl;
	QLabel* m_txtfile;

	void refreshItem();

protected Q_SLOTS:
	void recvFileSelect();
	void recvTextFinish();
};

class qui_resource
{
public:
	class qui_pixmap : public QPixmap
	{
	public:
		explicit qui_pixmap(uint32_t id, QImage* image);
		~qui_pixmap();

		uint32_t _id() { return m_id; }
		QImage* _image() { return m_image; }
	protected:
		uint32_t m_id;
		QImage* m_image;
	};

	static qui_resource& getInstance();

	QImage* getImage(uint32_t id);

protected:
	explicit qui_resource();
	~qui_resource();

	QList<qui_pixmap*> m_images;
};

#endif
