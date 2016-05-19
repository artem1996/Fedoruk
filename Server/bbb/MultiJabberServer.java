package Server.bbb;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created by Nikita on 17.05.2016.
 */

public class MultiJabberServer {
    static final int PORT = 8080;

    public static void main(String[] args) throws IOException {
        ServerSocket s = new ServerSocket(PORT);
        System.out.println("Server Started");
        try {
            while (true) {
                // Блокируется до возникновения нового соединения:
                Socket socket = s.accept();
                try {
                    new ServeOneJabber(socket);
                }
                catch (IOException e) {
                    // Если завершится неудачей, закрывается сокет,
                    // в противном случае, нить закроет его:
                    socket.close();
                }
            }
        }
        finally {
            s.close();
        }
    }














    static class ServeOneJabber extends Thread {
        private Socket socket;
        private BufferedReader in;
        private PrintWriter out;

        public ServeOneJabber(Socket s) throws IOException {
            socket = s;
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            // Включаем автоматическое выталкивание:
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket
                    .getOutputStream())), true);
            // Если любой из вышеприведенных вызовов приведет к
            // возникновению исключения, то вызывающий отвечает за
            // закрытие сокета. В противном случае, нить
            // закроет его.
            start(); // вызываем run()
        }

        public void run() {
            try {
                while (true) {
                    String str = in.readLine();
                    if (str.equals("END"))
                        break;
                    System.out.println("Echoing: " + str);
                    out.println(str);
                }
                System.out.println("closing...");
            }
            catch (IOException e) {
                System.err.println("IO Exception");
            }
            finally {
                try {
                    socket.close();
                }
                catch (IOException e) {
                    System.err.println("Socket not closed");
                }
            }
        }
    }


} // /:~
