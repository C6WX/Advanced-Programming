using Unity.AppUI.UI;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections;
using Unity.VisualScripting;

public class VictoryPanel : MonoBehaviour
{
    public void RestartPressed()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
    }

    public void FeedbackPressed()
    {
        Application.OpenURL("https://docs.google.com/forms/d/e/1FAIpQLSfyJOA72ZC8wcbG2nP_VR0p_gDZRJoI_Ge51D0Yn6US1igUyw/viewform?usp=header");
    }

    public void QuitGame()
    {
        Application.Quit();
    }
}
