#include "pch.h"

std::map<std::tstring, std::tstring> g_keys;

void init_keys()
{
    g_keys[_T("Windows 10 Professional")]                   = _T("W269N-WFGWX-YVC9B-4J6C9-T83GX");
    g_keys[_T("Windows 10 Enterprise")]                     = _T("NPPR9-FWDCX-D2C8J-H872K-2YT43");
    g_keys[_T("Windows 8.1 Professional")]                  = _T("GCRJD-8NW9H-F2CDX-CCM8D-9D6T9");
    g_keys[_T("Windows 8.1 Enterprise")]                    = _T("MHF9N-XY6XB-WVXMC-BTDCT-MKKG7");
    g_keys[_T("Windows 7 Professional")]                    = _T("FJ82H-XT6CR-J8D7P-XQJJ2-GPDD4");
    g_keys[_T("Windows 7 Enterprise")]                      = _T("33PXH-7Y6KF-2VJC9-XBBR8-HVTHH");
    g_keys[_T("Windows Server 2016 Standard")]              = _T("WC2BQ-8NRM3-FDDYY-2BFGV-KHKQY");
    g_keys[_T("Windows Server 2016 Datacenter")]            = _T("CB7KF-BWN84-R7R2Y-793K2-8XDDG");
    g_keys[_T("Windows Server 2012 R2 Server Standard")]    = _T("D2N9P-3P6X9-2R39C-7RTCD-MDVJX");
    g_keys[_T("Windows Server 2012 R2 Datacenter")]         = _T("W3GGN-FT8W3-Y4M27-J84CP-Q3VJ9");
    g_keys[_T("Windows Server 2008 R2 Standard")]           = _T("YC6KT-GKW9T-YTKYR-T4X34-R7VHC");
    g_keys[_T("Windows Server 2008 R2 Enterprise")]         = _T("489J6-VHDMP-X63PK-3K798-CPX3Y");
}

bool get_product_name(std::tstring& product_name)
{

    LSTATUS result = ERROR_SUCCESS;

    HKEY   hKey = NULL;
    DWORD  size = 0;
    PTCHAR pszName = nullptr;
    
    __try
    {
        result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                              _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
                              0,
                              KEY_READ,
                              &hKey);

        if (result != ERROR_SUCCESS)
        {

            __leave;
        }


        result = RegQueryValueEx(hKey,
                                 _T("ProductName"),
                                 NULL,
                                 NULL,
                                 (LPBYTE)pszName,
                                 &size);

        if (result != ERROR_SUCCESS)
        {

            __leave;
        }


        pszName = (PTCHAR)malloc(size);

        if (pszName == nullptr)
        {

            __leave;
        }


        result = RegQueryValueEx(hKey,
                                 _T("ProductName"),
                                 NULL,
                                 NULL,
                                 (LPBYTE)pszName,
                                 &size);


        if (result != ERROR_SUCCESS)
        {

            return false;
        }

        product_name = pszName;

    }
    __finally
    {

        if (hKey != NULL)
        {

            RegCloseKey(hKey);
        }


        if (pszName != nullptr)
        {

            free(pszName);
        }
    }


    return result == ERROR_SUCCESS ? true : false;
}

bool get_key(std::tstring& product_name, std::tstring& key)
{
    auto find = g_keys.find(product_name);
    if (find == g_keys.end())
    {

        return false;
    }

    key = g_keys[product_name];

    return true;
}

bool execute_command(std::tstring command)
{

    std::tstring cmdexe;
    TCHAR szSysDir[MAX_PATH] = { 0 };

    STARTUPINFO           si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION   pi = { 0 };


    if (GetSystemDirectory(szSysDir, MAX_PATH) == 0)
    {
        
        return false;
    }

    cmdexe = szSysDir;

    SetCurrentDirectory(szSysDir);

    auto result = CreateProcess(_T("cscript.exe"),
                                (LPWSTR)(command).c_str(), 
                                NULL, 
                                NULL, 
                                FALSE, 
                                NULL,
                                NULL, 
                                NULL, 
                                &si, 
                                &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);

    return result ? true : false;
}

bool execute_kms_activate(std::tstring& key)
{

    byte result = 0;

    result += execute_command(_T("//Nologo slmgr.vbs /ipk ") + key);
    result += execute_command(_T("//Nologo slmgr.vbs /skms kms.03k.org"));
    result += execute_command(_T("//Nologo slmgr.vbs /ato"));


    return result == 3 ? true : false;
}

int main()
{

    init_keys();


    bool result = false;


    std::tstring key;
    std::tstring product_name;
    
    result = get_product_name(product_name);

    if (result == false)
    {
        
        std::tcout << _T("get windows product name fail...") << std::endl;
        return 0;
    }


    result = get_key(product_name, key);

    if (result == false)
    {

        std::tcout << _T("not support ") << product_name << _T("...") << std::endl;
        return 0;
    }


    result = execute_kms_activate(key);

    if (result == false)
    {

        std::tcout << _T("activate fail...") << std::endl;
        return 0;
    }


    std::tcout << _T("success!");
    system("pause");

    return 0;
}
