package Cell;


/**
* Cell/TubeCallbackOperations.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from Cell.idl
* Thursday, May 19, 2016 12:28:51 PM MSK
*/


/* Интерфейс обратного вызова трубки */
public interface TubeCallbackOperations 
{

  /* Принять сообщение message от номера fromNum */
  int sendSMS (String fromNum, String message);

  /* Вернуть свой номер */
  String getNum ();
} // interface TubeCallbackOperations
