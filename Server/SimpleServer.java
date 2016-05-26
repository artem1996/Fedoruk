package Server;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;


/**
 * Created by Nikita on 21.11.2015.
 */
public class SimpleServer implements Runnable{
    private static boolean hasNextMessage;
    private static String msg;
    @Override
    public void run() {
        try {
            ServerSocket ss = new ServerSocket(1488); //make readthread and writethread separate
            Socket s = ss.accept();
            new Thread(new ReadThread(s)).start();
            new Thread(new WriteThread(s)).start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public SimpleServer() {
        msg = "";
        hasNextMessage = false;
    }

    public static void main(String[] args) {
        SimpleServer server = new SimpleServer();
        new Thread(server).start();
    }
    private static class ReadThread implements Runnable{
        Socket socket;
        BufferedReader reader;
        String message;
        public ReadThread(Socket socket) {
            this.socket = socket;
            message = "";
            try {
                reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            try {
                System.out.println("run run run");
                String line;
                while (!Thread.interrupted()) {
                        line = reader.readLine();
                        System.out.println(line); //stop deving here
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        public String getMessage(){
            return message;
        }
    }
    private static class WriteThread implements Runnable{
        public WriteThread(Socket socket) {
        }

        @Override
        public void run() {

        }
    }
}
