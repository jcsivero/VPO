#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    focus_("Ninguna imagen Seleccionada"),
    dialog_(nullptr)
{
    ui->setupUi(this);
    statusBar();
    statusBar()->addPermanentWidget(&statusPermanentMessage_);
    statusBar()->addWidget(&information_);
    statusPermanentMessage_.setAlignment(Qt::AlignLeft);
    information_.setAlignment(Qt::AlignRight);
    statusPermanentMessage_.setText(focus_);

}

MainWindow::~MainWindow()
{
qDebug() << "Destrucyendo mainwindow";
    delete ui;

if (dialog_ != nullptr)
   delete dialog_;

qDebug() << "Destructor mainwindow";
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}



void MainWindow::on_actionExit_triggered()
{
    close();
}



void MainWindow::on_actionOpenFiles_triggered()
{
 QStringList filenames;

   filenames = QFileDialog::getOpenFileNames(nullptr, "Abrir Archivo",QString(),"Multimedia (*.*)");

   for (int i=0; i < filenames.size(); i++)
        {
                QFile file(filenames[i]);
                 if (file.exists())
                       {
                        Image *imagen= new Image(filenames[i],this);
                        ui->menuVentanas->addAction(imagen->toggleViewAction());
                        addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea,imagen,Qt::Orientation::Vertical);
                        images_.insert(filenames[i],imagen);
                       }
                else
                        QMessageBox::warning(nullptr,"Atención",QString("El archivo %1 no existe.").arg(filenames[i]));

        }


}


void MainWindow::on_actionSaveFile_triggered()
{

}
///Devuelve puntero clase Image, con la imagen encontrada. nullptr si no se encontr�.
/// name Qstring con nombre de la imagen.
/// \brief findImage
/// \param name
/// \return
///
Image * MainWindow::findImage(QString name)
{
  QMap<QString, Image *>::iterator it = images_.begin();
  bool encontrado = false;
  Image *  imagen = nullptr;

  while ((it!=images_.end()) && (!encontrado))
    {
      if (it.key()==name)
        {
              encontrado = true;
              imagen = it.value();
        }
      ++it;
    }

  if (!encontrado)
   QMessageBox::warning(this,"No Encontrado",QString("Imagen no Seleccionada o No encontrada"));

  return imagen;
}
///Devuelve puntero Image con una copia de la imagen buscada, con un nuevo t�tulo.
/// Si toHistograma es true , devuelve una claes Imagen que contiene el histrograma absoluto o acumulado seg�n tipoHistograma
/// \brief findImage
///
Image * MainWindow::findImageAndNew(QString name, QString newTitle, bool toHistograma,bool tipoHistograma)
{
  QMap<QString, Image *>::iterator it = images_.begin();
  bool encontrado = false;
  Image *  imagen = nullptr;

  while ((it!=images_.end()) && (!encontrado))
    {
      if (it.key()==name)
        {
              encontrado = true;
              if (!toHistograma)
                  imagen = new Image(QString("%1_%2").arg(name).arg(newTitle),it.value()->getImage(),this);
              else
                  imagen = new Image(QString("%1_%2").arg(name).arg(newTitle),it.value()->toHistograma(tipoHistograma),this);

              images_.insert(QString("%1_%2").arg(name).arg(newTitle),imagen);
              ui->menuVentanas->addAction(imagen->toggleViewAction());
              addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea,imagen,Qt::Orientation::Vertical);
        }
      ++it;
    }

  if (!encontrado)
   QMessageBox::warning(this,"No Encontrado",QString("Imagen no Seleccionada o No encontrada"));

  return imagen;
}


void MainWindow::on_actionHistograma_absoluto_triggered()
{
  findImageAndNew(focus_,"Histograma Absoluto",true,false);
}

void MainWindow::on_actionHistograma_Acumulativo_triggered()
{ 
  findImageAndNew(focus_,"Histograma Acumulativo",true,true);

}



void MainWindow::on_actionBrilloyContraste_triggered()
{
  Image * borrador = findImageAndNew(focus_,"Brillo y Contraste");

  if (borrador != nullptr)
    {
      if (dialog_!=nullptr)
        delete dialog_;

       dialog_ = new BrilloYContraste(this);
       dialog_->setWindowTitle(QString("Brillo y Contraste para : %1").arg(focus_));
       dialog_->setImage(borrador);
       dialog_->show();

    }

}


void MainWindow::on_actionEntropia_triggered()
{

  if (findImage(focus_) != nullptr)
          QMessageBox::information(this,"Entropia",QString("Valor para %1: %2").arg(focus_).arg(findImage(focus_)->entropia_));


}

void MainWindow::on_actionGamma_triggered()
{ 

  Image * borrador = findImageAndNew(focus_,"Gamma");

  if (borrador != nullptr)
    {
      QDialog * dialogGamma = new QDialog(this);
      QDoubleSpinBox * valor = new QDoubleSpinBox(dialogGamma);
      QLabel * label = new QLabel(dialogGamma);
      QHBoxLayout * layout = new QHBoxLayout(dialogGamma);

      dialogGamma->setWindowTitle(QString("Valor Gamma para %1").arg(focus_));
      label->setText("Introduzca valor para la funci�n Gamma\n\nValores > 1 Mejora contraste en zona clara\nValores < 1 Mejora contraste en la zona oscura");
      layout->addWidget(label);
      layout->addWidget(valor);
      dialogGamma->setLayout(layout);
      valor->setMinimum(0);
      valor->setMaximum(255.0);
      valor->setSingleStep(0.1);
      valor->setValue(1.0);


      dialogGamma->show();

      QImage * imageForKeepOriginal = borrador->getImage();

      connect(valor,QOverload<double>::of(&QDoubleSpinBox::valueChanged),[=](double d){

          borrador->setImage(*imageForKeepOriginal);
          borrador->funcionGamma(d);

      });


      connect(dialogGamma,(&QDialog::finished),[=](int result){

        delete imageForKeepOriginal;

      });


    }

}

void MainWindow::on_actionEcualizar_triggered()
{
 Image * borrador;
 borrador = findImageAndNew(focus_,"Ecualizada");

 if (borrador != nullptr)
      borrador->toEcualizer();

}


///
/// \brief MainWindow::grayScale
/// \param ntsc
///
void MainWindow::grayScale(bool ntsc,bool ochobits)
{
  Image * borrador;
  if (ochobits)
    {
     borrador = findImageAndNew(focus_,"Gris 8 Bits");
     if (borrador!= nullptr)
       borrador->toGray(ntsc,true);
     }

  else
    {
    borrador = findImageAndNew(focus_,"Gris RGB32 Bits");
    if (borrador!= nullptr)
      borrador->toGray(ntsc,false);
    }



}
void MainWindow::on_actionNTSC8Bits_triggered()
{
    grayScale(true,true);
}

void MainWindow::on_actionPAL8Bits_triggered()
{
    grayScale(false,true);
}

void MainWindow::on_actionRGB32_NTSC_triggered()
{
    grayScale(true,false);
}

void MainWindow::on_actionRGB32_PAL_triggered()
{
    grayScale(false,false);
}



void MainWindow::on_actionEspecificacion_triggered()
{
  Image * borrador;

  borrador= findImageAndNew(focus_,"Histograma Especificado");

  if (borrador!= nullptr)
    {
      QDialog * dialogEspecificarHistograma = new QDialog(this);
      QLabel * label = new QLabel(dialogEspecificarHistograma);
      QComboBox * comboBoxTargetHistograma = new QComboBox(dialogEspecificarHistograma);
      QPushButton * pushButtonApply = new QPushButton(dialogEspecificarHistograma);
      QVBoxLayout * layout = new QVBoxLayout(dialogEspecificarHistograma);


      QMap<QString, Image *>::iterator it = images_.begin();

      while (it!=images_.end())
        {
          if ((it.key()!=focus_) && (!it.key().contains("histograma",Qt::CaseInsensitive))) ///agrego al combo solo las im�genes que no sean esta misma y que no sean histogramas
              comboBoxTargetHistograma->addItem(it.key());
          ++it;
        }

      if (comboBoxTargetHistograma->count()!=0)
      {

      pushButtonApply->setText("Aplicar");
      label->setText("Selecciona una imagen desde las ya abiertas.\nEl Histograma Destino ser� el que tenga la imagen seleccionada.");
      layout->addWidget(label);
      layout->addWidget(comboBoxTargetHistograma);
      layout->addWidget(pushButtonApply);


      dialogEspecificarHistograma->setWindowTitle("Histograma Destino");


      connect(pushButtonApply,(&QPushButton::clicked),[=](bool checked){

          borrador->toHistogramaEspecificado(findImage(comboBoxTargetHistograma->currentText()));
          dialogEspecificarHistograma->close();

      });


      dialogEspecificarHistograma->exec();


      }
      else
      {
        QMessageBox::warning(nullptr,"Atención:Falta imagen destino","Debe abrir por lo menos otra imagen de la que obtener su histograma");
        deleteImage(focus_ + "_Histograma Especificado");
      }

    }

}
/// Elimino del vector de im�genes la imagen indicada o todas si name est� vac�o
/// \brief MainWindow::deleteImage
/// \param name
///
void MainWindow::deleteImage(QString name)
{
   QMap<QString, Image *>::iterator it = images_.begin();
  Image *  imagen = nullptr;

qDebug() << name;

  while (it!=images_.end())
    {
      if (name.isEmpty())
          {
            delete it.value();
            images_.erase(it);

          }
      else  if (it.key()==name)
      {

              delete it.value();
              images_.erase(it);

      }
      ++it;
    }

}

void MainWindow::on_actionInformacion_triggered()
{
  Image *borrador;
  borrador = findImage(focus_);

  QString information;

  information = "\nNombre Imagen : " + focus_;
   if (borrador!=nullptr)
     {
       if (borrador->isGray_)
       {

           if (borrador->format_==QImage::Format_Indexed8)
            information = information + "\n\nImagen Gris de 8 Bits";
          else
          information = information + "\n\nImagen Gris RGB de 32 Bits";
           information = information + "\nBrillo: " + QString::number(borrador->brillo_gray_);
           information = information + "\nContraste: " + QString::number(borrador->contraste_gray_);


           information =information + "\n\nValor inferior de Gris: " + QString::number(borrador->grayValueMin_) + "\nValor superior de Gris: " + QString::number(borrador->grayValueMax_);
           information =information + "\nTono de Gris con mayor cantidad de pixels: " + QString::number(borrador->toneGrayWidthMorePixels_) + " con " + QString::number(borrador->histograma_[borrador->toneGrayWidthMorePixels_].countGray_) + "\nTono de Gris con menor cantidad de pixels: " + QString::number(borrador->toneGrayWidthLessPixels_) + " con " + QString::number(borrador->histograma_[borrador->toneGrayWidthLessPixels_].countGray_) ;

        }
       else
          {
          information = information + "\n\nImagen Color RGB de 32 Bits";

          information = information + "\n\nBrillo Tono Rojo: " + QString::number(borrador->brillo_red_);
          information = information + "\nContraste Tono Rojo: " + QString::number(borrador->contraste_red_);
          information =information + "\nValor inferior de Rojo: " + QString::number(borrador->redValueMin_) + "\nValor superior de Rojo: " + QString::number(borrador->redValueMax_);
          information =information + "\nTono de Rojo con mayor cantidad de pixels: " + QString::number(borrador->toneRedWidthMorePixels_) + " con " + QString::number(borrador->histograma_[borrador->toneRedWidthMorePixels_].countRed_) + "\nTono de Rojo con menor cantidad de pixels: " + QString::number(borrador->toneRedWidthLessPixels_) + " con " + QString::number(borrador->histograma_[borrador->toneRedWidthLessPixels_].countRed_) ;


          information = information + "\n\nBrillo Tono Verde: " + QString::number(borrador->brillo_green_);
          information = information + "\nContraste Tono Verde: " + QString::number(borrador->contraste_green_);
          information =information + "\nValor inferior de Verde: " + QString::number(borrador->greenValueMin_) + "\nValor superior de Verde " + QString::number(borrador->greenValueMax_);
          information =information + "\nTono de Verde con mayor cantidad de pixels: " + QString::number(borrador->toneGreenWidthMorePixels_) + " con " + QString::number(borrador->histograma_[borrador->toneGreenWidthMorePixels_].countGreen_) + "\nTono de Verde con menor cantidad de pixels: " + QString::number(borrador->toneGreenWidthLessPixels_) + " con " + QString::number(borrador->histograma_[borrador->toneGreenWidthLessPixels_].countGreen_) ;



          information = information + "\n\nBrillo Tono Azul: " + QString::number(borrador->brillo_blue_);
          information = information + "\nContraste Tono Azul: " + QString::number(borrador->contraste_blue_);
          information =information + "\nValor inferior de Azul: " + QString::number(borrador->blueValueMin_) + "\nValor superior de Azul: " + QString::number(borrador->blueValueMax_);
          information =information + "\nTono de Azul con mayor cantidad de pixels: " + QString::number(borrador->toneBlueWidthMorePixels_) + " con " + QString::number(borrador->histograma_[borrador->toneBlueWidthMorePixels_].countBlue_) + "\nTono de Azul con menor cantidad de pixels: " + QString::number(borrador->toneBlueWidthLessPixels_) + " con " + QString::number(borrador->histograma_[borrador->toneBlueWidthLessPixels_].countBlue_) ;

          }

       information = information + QString::fromUtf8("\n\nValor entrop�a: ") + QString::number(borrador->entropia_);
       information = information + "\n\Ancho: " + QString::number(borrador->width_) + " pixels";
       information = information + "\nAlto: " + QString::number(borrador->height_) + " pixels";
       information = information + "\nTotal de Pixels: " + QString::number(borrador->height_ * borrador->width_) + " pixels";



       QMessageBox::information(this,QString::fromUtf8("Informaci�n"),information);
     }


}

