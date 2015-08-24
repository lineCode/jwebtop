package org.jwebtop;

import java.awt.Component;
import java.io.File;
import java.io.IOException;

/**
 * 与JWebTop交互的JNI接口java端
 * 
 * @author washheart@163.com
 */
public final class JWebTopNative {
	private static ClassLoader commonlassLoader;

	private static native void nExit();

	private static native void nCreateJWebTop(String processPath, String configFile);

	private static native long nCreateBrowser(String appFile, long parenHwnd, String url, String title, String icon, int x, int y, int w, int h);

	private static native void nCloseBrowser(long browserHWnd);

	private static native String nExecuteJSONWait(long browserHWnd, String jsonstring);

	private static native void nExecuteJSONNoWait(long browserHWnd, String jsonstring);

	private static native String nExecuteJSWait(long browserHWnd, String script);

	private static native void nExecuteJSNoWait(long browserHWnd, String script);

	private static native void nSetSize(long browserHwnd, int w, int h);

	private static native int[] nGetSize(long browserHwnd);

	private static native int[] nGetScreenSize();

	private static native void nSetLocation(long browserHwnd, int xOnScreen, int yOnScreen);

	private static native int[] nGetLocation(long browserHwnd);

	private static native void nSetBound(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h);

	private static native int[] nGetBound(long browserHwnd);

	private static native int[] nGetWindowClient(long browserHwnd);

	private static native void nBringToTop(long browserHWnd);

	private static native void nFocus(long browserHWnd);

	private static native void nHide(long browserHWnd);

	private static native void nMax(long browserHWnd);

	private static native void nMini(long browserHWnd);

	private static native void nRestore(long browserHWnd);

	private static native void nSetTopMost(long browserHWnd);

	private final static JWebTopNative INSTANCE = new JWebTopNative();

	private JWebtopJSONDispater jsonHandler = null;

	public static JWebTopNative getInstance() {
		return INSTANCE;
	}

	private JWebTopNative() {}

	/**
	 * 启动JWebTop进程，不立即创建浏览器。<br/>
	 * 此方法仅在第一次调用时创建进程，再次调用不执行任何操作
	 * 
	 * @param processPath
	 *            待执行的JWebTop.exe的全路径。比如d:\c\JWebTop.exe，当然JWebTop可以命名为其他名字。
	 * @param configFile
	 *            此配置文件用于启动JWebTop进程（但，不立刻创建浏览器窗口，即使指定了url参数）
	 * @return
	 */
	public void createJWebTop(String processPath, String configFile) {
		if (jsonHandler == null) throw new JWebTopException("必须设置jsonHandler。");
		String processPath2, appfile2;
		try {
			commonlassLoader = JWebTopNative.class.getClassLoader();
			File file = new File(processPath);
			if (!file.exists()) throw new JWebTopException("找不到可执行文件：" + processPath);
			processPath2 = file.getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
			appfile2 = new File(configFile).getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
			nCreateJWebTop(processPath2, appfile2);
		} catch (IOException e) {
			throw new JWebTopException("无法创建JWebTop", e);
		}
	}

	public long createBrowser(final String appfile) throws IOException {
		return createBrowser(appfile, 0, null, null, null, -1, -1, -1, -1);
	}

	public long createBrowser(final String appfile, final long parenHwnd) throws IOException {
		return createBrowser(appfile, parenHwnd, null, null, null, -1, -1, -1, -1);
	}

	public long createBrowser(final String appfile, final long parenHwnd
	// 以下参数会替换appfile中的相应参数
			, final String url // --------要打开的链接地址
			, final String title // ------窗口名称
			, final String icon // -------窗口图标
			, final int x, final int y // 窗口左上角坐标,当值为-1时不启用此变量
			, final int w, final int h // 窗口的宽、高，当值为-1时不启用此变量
	) {
		String appfile2;
		try {
			appfile2 = new File(appfile).getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
		} catch (IOException e) {
			throw new JWebTopException("无法创建Browser", e);
		}
		return nCreateBrowser(appfile2, parenHwnd, url, title, icon, x, y, w, h);
	}

	/**
	 * 关闭指定浏览器
	 * 
	 * @param browserHWnd
	 */
	public static void closeBrowser(long browserHWnd) {
		nCloseBrowser(browserHWnd);
	}

	// 关闭JWebTop的进程
	public void exit() {
		nExit();
	}

	/**
	 * 执行js，发送后等待数据的返回。<br/>
	 * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	 * 
	 * @param jsonstring
	 * @return 返回是JSON字符串数据
	 */
	public static String executeJSON_Wait(long browserHWnd, String jsonstring) {
		return nExecuteJSONWait(browserHWnd, jsonstring);
	}

	/**
	 * 执行js，但不等待数据的返回。<br/>
	 * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	 * 
	 * @param jsonstring
	 */
	public static void executeJSON_NoWait(long browserHWnd, String jsonstring) {
		nExecuteJSONNoWait(browserHWnd, jsonstring);
	}

	/**
	 * 执行js，并等待数据的返回。<br/>
	 * 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
	 * 
	 * @param script
	 * @return 返回是字符串数据
	 */
	public static String executeJS_Wait(long browserHWnd, String script) {
		return nExecuteJSWait(browserHWnd, script);
	}

	/**
	 * 执行js，但不等待数据的返回。 <br/>
	 * 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
	 * 
	 * @param script
	 * @return
	 */
	public static void executeJS_NoWait(long browserHWnd, String script) {
		nExecuteJSNoWait(browserHWnd, script);
	}

	String dispatch(String json) throws IOException {
		jsonHandler.resetThreadClassLoader();
		return jsonHandler.dispatcher(json);
	}

	public void setJsonHandler(JWebtopJSONDispater jsonHandler) {
		this.jsonHandler = jsonHandler;
	}

	/**
	 * 此方法不可以被混淆，其会被DLL调用
	 * 
	 * @param json
	 * @return
	 */
	static String invokeByJS(String json) {// 可以说private，但用public可以避免被优化掉
		try {
			return INSTANCE.dispatch(json);
		} catch (Throwable e) {
			e.printStackTrace();
			return "{success:false,msg:\"调用Java失败" + e.getMessage() + "\"}";
		}
	}

	/**
	 * 设置指定窗口的大小
	 * 
	 * @param browserHwnd
	 * @param w
	 * @param h
	 */
	public static void setSize(long browserHwnd, int w, int h) {
		if (browserHwnd != 0) nSetSize(browserHwnd, w, h);
	}

	/**
	 * 得到指定窗口的大小
	 * 
	 * @param browserHwnd
	 * @return int[0]=宽，int[1]=高
	 */
	private static int[] getSize(long browserHwnd) {
		return nGetSize(browserHwnd);
	}

	/**
	 * 得到屏幕可用区域的大小
	 * 
	 * @return int[0]=宽，int[1]=高
	 */
	private static int[] getScreenSize() {
		return nGetScreenSize();
	}

	/**
	 * 设置指定窗口的位置
	 * 
	 * @param browserHwnd
	 * @param xOnScreen
	 * @param yOnScreen
	 */
	public static void setLocation(long browserHwnd, int xOnScreen, int yOnScreen) {
		if (browserHwnd != 0) nSetLocation(browserHwnd, xOnScreen, yOnScreen);
	}

	/**
	 * 得到窗口在屏幕上的位置
	 * 
	 * @param browserHwnd
	 * @return int[0]=x，int[1]=y
	 */
	private static int[] getLocation(long browserHwnd) {
		return nGetLocation(browserHwnd);
	}

	/**
	 * 移动并重设大小(性能较高， 但此方法有时有点问题：刷新不及时)
	 * 
	 * @param browserHwnd
	 * @param xOnScreen
	 * @param yOnScreen
	 * @param w
	 * @param h
	 */
	public static void setBound(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h) {
		if (browserHwnd != 0) nSetBound(browserHwnd, xOnScreen, yOnScreen, w, h);
	}

	/**
	 * 得到窗口在屏幕的坐标信息
	 * 
	 * @param browserHwnd
	 * @return int[0]=x，int[1]=y，int[2]=宽，int[3]=高
	 */
	private static int[] getBound(long browserHwnd) {
		return nGetBound(browserHwnd);
	}

	// 窗口移到最顶层
	private static void bringToTop(long browserHWnd) {
		nBringToTop(browserHWnd);
	}

	// 使窗口获得焦点
	private static void focus(long browserHWnd) {
		nBringToTop(browserHWnd);
	}

	// 隐藏窗口
	private static void hide(long browserHWnd) {
		nHide(browserHWnd);
	}

	// 最大化窗口
	private static void max(long browserHWnd) {
		nMax(browserHWnd);
	}

	// 最小化窗口
	private static void mini(long browserHWnd) {
		nMini(browserHWnd);
	}

	// 还原窗口，对应于hide函数
	private static void restore(long browserHWnd) {
		nRestore(browserHWnd);
	}

	// 窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数
	private static void setTopMost(long browserHWnd) {
		nSetTopMost(browserHWnd);
	}

	/**
	 * 得到某Java控件对应的句柄HWND
	 * 
	 * @param target
	 *            给定的控件
	 * @return 控件windowsID
	 */
	public static long getWindowHWND(Component target) {
		return ((sun.awt.windows.WComponentPeer/* 此类来自JDK，纯jre不行 */) target.getPeer()).getHWnd();
	}

	/**
	 * 获取指定窗口的客户区坐标信息，返回大小为4的数组，依次为右左上角x、y、右下角x、y
	 * 
	 * @param hWnd
	 *            窗口句柄
	 * @return int[left,top,right,bottom]
	 */
	public static int[] getWindowClient(long hWnd) {
		return nGetWindowClient(hWnd);
	}
}