#pragma once
#include <QObject>
#include <QTimer>

#include <AL/al.h>
#include <AL/alc.h>

class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    AudioRecorder();
    void StartRecord();
    const QString& StopRecord();
private slots:
    void updateRecord();
private:
    QTimer m_updateRecordTimer;
    ALCdevice* m_recordDevice;

    ALbyte m_recordBuffer[22050];
    QString m_filePath;
};
