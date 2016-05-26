package Server;
import java.util.ArrayList;
import java.util.Arrays;

public class lab {
    public static void main(String[] args) {
        String line = "10 to 5:asd";
        int targetID = Integer.parseInt(line.split(" to ")[1].split(":")[0]);
        System.out.println(targetID);
    }
}