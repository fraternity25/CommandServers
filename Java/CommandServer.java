import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.ProcessBuilder.Redirect;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class CommandServer 
{
    public static void main(String[] args) 
    {
        int port = 10000; // 80;
        String publicIp = "localhost"; // "78.175.229.225"; "0.0.0.0"; "192.168.1.118";
        
        // Detect the operating system
        String os = System.getProperty("os.name").toLowerCase();
        
        try (ServerSocket serverSocket = new ServerSocket(port, 2, InetAddress.getByName(publicIp))) 
        {
            if (publicIp.equals("localhost")) 
            {
                publicIp = InetAddress.getLocalHost().getHostAddress();
            }
            
            System.out.println("Server is running on " + publicIp + ":" + port);
            
            while (true) 
            {
                Socket clientSocket = serverSocket.accept();
                new ClientHandler(clientSocket, os).start();
            }
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }
    }
}

class ClientHandler extends Thread 
{
    private Socket clientSocket;
    private String clientName;
    private String os;
    
    public ClientHandler(Socket socket, String os) 
    {
        this.clientSocket = socket;
        this.os = os;
    }
    
    public boolean system(String command, boolean redirectInput, boolean redirectOutput, StringBuilder output) 
    {
        boolean success = true;
        try 
        {
            ProcessBuilder builder;
            if (os.contains("win")) 
            {
                builder = new ProcessBuilder("cmd.exe", "/c", command);
            }
            else if (os.contains("nix") || os.contains("nux") || os.contains("mac")) 
            {
                builder = new ProcessBuilder("sh", "-c", command);
            }
            else if (os.contains("android")) 
            {
                builder = new ProcessBuilder("sh", "-c", command);
            }
            else if (os.contains("ios")) 
            {
                // iOS specific handling (usually not applicable for Java apps)
                builder = new ProcessBuilder("sh", "-c", command);
            }
            else 
            {
                throw new UnsupportedOperationException("Unsupported operating system: " + os);
            }
            
            if (redirectInput) 
            {
                builder.redirectInput(Redirect.INHERIT);
            }
            
            if (redirectOutput) 
            {
                builder.redirectOutput(Redirect.INHERIT);
                builder.redirectError(Redirect.INHERIT);
            }
            Process process = builder.start();
            
            // Capturing stdout
            StringBuilder stdout = new StringBuilder();
            if (output != null) 
            {
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) 
                {
                    stdout.append(line).append(System.lineSeparator());
                }
            }
            
            // Capturing stderr
            StringBuilder stderr = new StringBuilder();
            if (output != null) 
            {
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
                String line;
                while ((line = reader.readLine()) != null) 
                {
                    stderr.append(line).append(System.lineSeparator());
                }
            }
            
            process.waitFor();
            success = (process.exitValue() == 0);
            
            if (output != null) 
            {
                if (stdout.length() > 0) 
                {
                    output.append(stdout);
                }
                if (stderr.length() > 0) 
                {
                    output.append(stderr);
                }
            }
        }
        catch (Exception e) 
        {
            e.printStackTrace();
            success = false;
        }
        return success;
    }
    
    public void run() 
    {
        try (BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))) 
        {
            // Read the client's name first
            clientName = in.readLine();
            System.out.println("Client connected: " + clientName);
            
            String input;
            while ((input = in.readLine()) != null) 
            {
                System.out.println("Command from " + clientName + ": " + input);
                if (input.equals("exit")) 
                {
                    break;
                }
                else if (input.equals("cls") || input.equals("clear")) 
                {
                    system(input, false, true, null);
                    continue;
                }
                
                String prefix = os.contains("win") ? "" : "./";
                // System.out.print("output: ");
                system(prefix + "colorline.exe 1", false, true, null);
                System.out.print("output: ");
                StringBuilder output = new StringBuilder();
                boolean success = system(input, true, false, output);
                if (success) 
                {
                    system(prefix + "colorline.exe a", false, true, null);
                    System.out.println(output);
                    // System.out.println("Command executed successfully!");
                }
                else 
                {
                    system(prefix + "colorline.exe 4", false, true, null);
                    System.out.println(output);
                    // System.out.println("Command execution failed!");
                }
                system(prefix + "colorline.exe 7", false, true, null);
            }
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }
        finally 
        {
            try 
            {
                clientSocket.close();
            }
            catch (IOException e) 
            {
                e.printStackTrace();
            }
            System.out.println("Client disconnected: " + clientName);
        }
    }
}
