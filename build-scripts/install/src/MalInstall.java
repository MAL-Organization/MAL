import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.RandomAccessFile;
import java.nio.channels.FileLock;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.HashMap;
import java.util.Map;

import com.googlecode.CachedFileEntry;


public class MalInstall {

	@SuppressWarnings("unchecked")
	public static void main(String[] args) {
		System.out.println("Installing artifact in maven download plugin cache");
		// Load arguments
		String basedirPath = args[0];
		String repodirPath = args[1];
		String version = args[2];
		// Make sure repo exists
		File repodir = new File(repodirPath + "/.cache/maven-download-plugin/");
		System.out.println("Cache location: " + repodir.getAbsolutePath());
		if (!repodir.exists()) {
			if (!repodir.mkdir()) {
				System.out.println("Failed to make repodir");
				System.exit(1);
			}
		}
		// Make sure library exists
		File lib = new File(basedirPath + "/target/mal-" + version + "-library.zip");
		System.out.println("Library location: " + lib.getAbsolutePath());
		if (!lib.exists()) {
			System.out.println("Failed to find lib");
			System.exit(1);
		}
		// Copy library
		try {
			File destinationFile = new File(repodir, lib.getName());
			Files.copy(lib.toPath(), destinationFile.toPath(), StandardCopyOption.REPLACE_EXISTING);
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		// Edit cache
		Map<String, CachedFileEntry> index = null;
		File cache = new File(repodir, "index.ser");
        if (cache.isFile()) {
            try (FileLock lock = new RandomAccessFile(cache, "r").getChannel().lock(0, Long.MAX_VALUE, true)) {
                try (ObjectInputStream deserialize = new ObjectInputStream(new FileInputStream(cache))) {
                    index = (Map<String, CachedFileEntry>) deserialize.readObject();
                } catch (ClassNotFoundException e) {
					e.printStackTrace();
					System.exit(1);
				}
            } catch (FileNotFoundException e) {
				e.printStackTrace();
				System.exit(1);
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(1);
			}
		} else {
			index = new HashMap<>();
		}
        // Add/overwrtie entry
        CachedFileEntry entry = new CachedFileEntry();
        String url = "https://github.com/MAL-Organization/MAL/releases/download/mal-" + 
        			 version+ "/mal-" + version + "-library.zip";
        entry.fileName = lib.getName();
        index.put(url, entry);
        // Save
		if (cache.exists()) {
			try {
				cache.createNewFile();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(1);
			}
		}
		FileOutputStream out = null;
		try {
			out = new FileOutputStream(cache);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			System.exit(1);
		}
		try (ObjectOutputStream res = new ObjectOutputStream(out)) {
            try (FileLock lock = out.getChannel().lock()) {
                res.writeObject(index);
            }
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
	}

}
