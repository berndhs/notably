#include "notaconf.h"



namespace nota {

NotaConf::NotaConf ()
{
  mPath = QString ("/home/bernd/data/nota");
  mDatafile = QString ("nota.sql");
}


QString
NotaConf::DataFile ()
{
  return mPath + "/" + mDatafile;
}

}
