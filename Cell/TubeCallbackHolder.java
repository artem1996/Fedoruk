package Cell;

/**
* Cell/TubeCallbackHolder.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from Cell.idl
* Thursday, May 19, 2016 12:28:51 PM MSK
*/


/* Интерфейс обратного вызова трубки */
public final class TubeCallbackHolder implements org.omg.CORBA.portable.Streamable
{
  public Cell.TubeCallback value = null;

  public TubeCallbackHolder ()
  {
  }

  public TubeCallbackHolder (Cell.TubeCallback initialValue)
  {
    value = initialValue;
  }

  public void _read (org.omg.CORBA.portable.InputStream i)
  {
    value = Cell.TubeCallbackHelper.read (i);
  }

  public void _write (org.omg.CORBA.portable.OutputStream o)
  {
    Cell.TubeCallbackHelper.write (o, value);
  }

  public org.omg.CORBA.TypeCode _type ()
  {
    return Cell.TubeCallbackHelper.type ();
  }

}