package org.jwebtop.demos.ctrl;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.net.URL;
import java.util.LinkedList;
import java.util.List;

import org.jwebtop.JWebTopNative;
import org.jwebtop.JWebtopJSONDispater;

import com.alibaba.fastjson.JSONObject;

public class WithinSwingCtrl implements JWebtopJSONDispater {
	public interface DetailBrowserListener {
		void detailBrowserCreated(long detailHandler);
	}

	private long listHandler, detailHandler;
	private List<String> names = null;
	private DetailBrowserListener detailBrowserListener = null;
	private String currentNote;

	@Override
	public String dispatcher(String json) {
		// 注意：下面的JSON解析方式未考虑性能，如果JSON数据过大，建议采用jackson按流模式解析
		JSONObject jo = JSONObject.parseObject(json);
		String method = jo.getString("method");
		if ("initList".equals(method)) {
			if (names != null) return "{}";
			names = new LinkedList<String>();
			BufferedReader br = null;
			try {
				File f = getNotesFile();
				Reader fr = new FileReader(f);
				br = new BufferedReader(fr);
				String line = null;
				while ((line = br.readLine()) != null) {
					names.add(line);
				}
				br.close();
				fr.close();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				if (br != null) try {
					br.close();
				} catch (IOException e) {}
			}
			JSONObject rtn = new JSONObject();
			rtn.put("method", method);
			rtn.put("value", names);
			// JWebTopNative.executeJs(listHandler, rtn.toJSONString());
			return rtn.toJSONString();
		} else if ("getDetailAppFile".equals(method)) {
			return getDetailAppFile();
		} else if ("setDetailHandler".equals(method)) {
			this.detailHandler = jo.getLong("value");
			if (this.detailBrowserListener != null) {
				this.detailBrowserListener.detailBrowserCreated(this.detailHandler);
			}
		} else if ("showDetail".equals(method)) {
			String name = jo.getString("value");
			this.currentNote = name;
			showDetail(name);
		} else if ("saveNote".equals(method)) {
			saveNote(jo.getString("note"), jo.getString("value"));
		}
		return "";
	}

	private File getNotesFile() throws IOException {
		File f = new File("data/list.txt");
		if (!f.exists()) {
			f.getParentFile().mkdirs();
			f.createNewFile();
		}
		return f;
	}

	private File getNoteFile(String name) throws IOException {
		File f = new File("data/" + name + ".txt");
		if (!f.exists()) {
			f.getParentFile().mkdirs();
			f.createNewFile();
		}
		return f;
	}

	private void showDetail(String note) {
		JSONObject rtn = new JSONObject();
		rtn.put("method", "showDetail");
		String detail = null;
		if (note == null || note.trim().length() == 0) {
			detail = "请使用“添加日记”按钮新建日记";
		} else {
			detail = "在这里输入【" + note + "】日记的内容";
			File f = new File("data/" + note + ".txt");
			if (f.exists()) {
				BufferedReader br = null;
				StringBuffer sb = new StringBuffer();
				try {
					Reader fr = new FileReader(f);
					br = new BufferedReader(fr);
					int ch;
					while ((ch = br.read()) != -1) {
						sb.append((char) ch);
					}
					fr.close();
				} catch (IOException e) {
					e.printStackTrace();
				} finally {
					try {
						if (br != null) br.close();
					} catch (IOException e) {}
				}
				detail = sb.toString();
			}
		}
		rtn.put("value", detail);
		rtn.put("note", note);
		JWebTopNative.executeJs(detailHandler, rtn.toJSONString());
	}

	public String getListAppFile() {
		File f = new File("res/list/index.app");
		if (f.exists()) return f.getAbsolutePath();
		URL url = WithinSwingCtrl.class.getClassLoader().getResource("");
		return url.getFile() + "../../demos/WithinSwing/res/list/index.app";
	}

	public String getDetailAppFile() {
		File f = new File("res/detail/index.app");
		if (f.exists()) return f.getAbsolutePath();
		URL url = WithinSwingCtrl.class.getClassLoader().getResource("");
		String fs = url.getFile() + "../../demos/WithinSwing/res/detail/index.app";
		try {
			return new File(fs).getCanonicalPath();
		} catch (IOException e) {
			return fs;
		}
	}

	public String getCurrentNote() {
		return currentNote;
	}

	public DetailBrowserListener getDetailBrowserListener() {
		return detailBrowserListener;
	}

	public void setDetailBrowserListener(DetailBrowserListener detailBrowserListener) {
		this.detailBrowserListener = detailBrowserListener;
	}

	public long getDetailHandler() {
		return detailHandler;
	}

	public void setListHandler(long listHandler) {
		this.listHandler = listHandler;
	}

	private void saveNotes() {
		FileWriter fw = null;
		try {
			File f = getNotesFile();
			fw = new FileWriter(f);
			for (String n : names) {
				fw.write(n);
				fw.write("\r\n");
			}
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if (fw != null) fw.close();
			} catch (IOException e) {}
		}
	}

	public void addNote(String name) {
		if (this.names.contains(name)) return;
		this.names.add(name);
		saveNotes();
		JSONObject jo = new JSONObject();
		jo.put("method", "noteAdded");
		jo.put("value", name);
		JWebTopNative.executeJs(listHandler, jo.toJSONString());
	}

	public void delNote() {
		int idx = this.names.indexOf(currentNote);
		if (idx == -1) return;
		this.names.remove(currentNote);
		saveNotes();
		JSONObject jo = new JSONObject();
		jo.put("method", "noteRemoved");
		jo.put("value", this.currentNote);
		this.currentNote = null;
		if (idx > 0) idx = idx - 1;
		if (idx < this.names.size()) {
			jo.put("newSel", this.names.get(idx));
		} else {
			showDetail(null);
		}
		JWebTopNative.executeJs(listHandler, jo.toJSONString());
	}

	public void saveNote(String note, String content) {
		if (note != null) {
			FileWriter fw = null;
			try {
				File f = getNoteFile(note);
				fw = new FileWriter(f);
				fw.write(content);
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				try {
					if (fw != null) fw.close();
				} catch (IOException e) {}
			}
		}
	}

	public void notifyWillClose() {
		JSONObject jo = new JSONObject();
		jo.put("method", "willClose");
		String json = jo.toJSONString();
		JWebTopNative.executeJs(detailHandler, json);
		JWebTopNative.executeJs(listHandler, json);
	}
}