#include <QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include <QTcpServer>
#include <QtConcurrent/QtConcurrent>
#include <QImage>
#include <QImageReader>
#include <QString>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto formats = QImageReader::supportedImageFormats();
    qInfo() << "supported image formats:" << formats;

    QHttpServer server;

    server.route("/", [&app]() {
        return app.applicationName();
     });

    // QSvg can be used to validate if input is valid svg image
    server.route("/upload/<arg>", [](const QString fileId, const QHttpServerRequest &req) {
        if (fileId.isEmpty())
            return QHttpServerResponse("Invalid filename provided", QHttpServerResponse::StatusCode::BadRequest);

        // QProcessEnvironment to be used instead for data volume path
        QSaveFile saveFile(QString("/efs/%1.svg").arg(fileId));
        saveFile.open(QSaveFile::OpenModeFlag::WriteOnly);
        saveFile.write(req.body());
        saveFile.commit();

        return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
    });

    server.route("/resize/<arg>/<arg>/<arg>", [&formats] (const QString fileId, const int w, const int h, const QHttpServerRequest &req) {
        return QtConcurrent::run([fileId, w, h, &req, &formats] () {
            QImage image(QString("/efs/%1.svg").arg(fileId));

            if (image.isNull())
                return QHttpServerResponse("Invalid image requested", QHttpServerResponse::StatusCode::BadRequest);

            QString format = req.query().queryItemValue("format").toLower();
            if (format.isEmpty())
                format = "webp";

            if (formats.contains(format)) {
                image = image.scaled(QSize(w, h), Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);

                QByteArray ba;
                QBuffer buffer(&ba);
                buffer.open(QIODevice::WriteOnly);
                image.save(&buffer, format.toLocal8Bit());

                // content-type for svg is 'svg+xml'
                return QHttpServerResponse(QString("image/%1").arg(format).toLocal8Bit(), ba, QHttpServerResponse::StatusCode::Ok);
            }

            return QHttpServerResponse("Please use one of [jpg,png,webp] formats", QHttpServerResponse::StatusCode::BadRequest);
        });
    });

    auto tcpserver = new QTcpServer();
    if (!tcpserver->listen(QHostAddress::Any, 8080)) {
        delete tcpserver;
        qCritical() << "Unable to start HTTP server on provided port 8080";
        return -1;
    }

    server.bind(tcpserver);

    qInfo().noquote().nospace() << "Listening on " << tcpserver->serverAddress().toString() << ":" << tcpserver->serverPort();

    return app.exec();
}
