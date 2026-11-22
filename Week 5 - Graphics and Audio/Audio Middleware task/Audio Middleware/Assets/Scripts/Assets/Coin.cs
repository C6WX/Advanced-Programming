using UnityEngine;
using FMODUnity;
public class Coin : MonoBehaviour
{
    [SerializeField] private EventReference coinCollectedSound;
    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            AudioManager.instance.PlayOneShot(coinCollectedSound, this.transform.position);
            Destroy(gameObject);
        }
    }
}
