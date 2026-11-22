using UnityEngine;
using UnityEngine.InputSystem;
using FMODUnity;

[RequireComponent(typeof(Rigidbody))]
public class Player : MonoBehaviour
{
    [Header("Audio")]
    [SerializeField] private EventReference jumpSound;

    [Header("Movement")]
    [SerializeField] private float moveSpeed = 8f;
    [SerializeField] private float jumpForce = 10f;

    [Header("Ground Check")]
    [SerializeField] private Transform groundCheck;
    [SerializeField] private float groundCheckRadius = 0.12f;
    [SerializeField] private LayerMask groundLayer;

    private Rigidbody rb;
    private float moveInput;
    private bool isGrounded;

    private void Awake()
    {
        rb = GetComponent<Rigidbody>();
    }

    private void Update()
    {
        moveInput = 0f;

        if (Keyboard.current != null)
        {
            if (Keyboard.current.aKey.isPressed || Keyboard.current.leftArrowKey.isPressed)
                moveInput -= 1f;
            if (Keyboard.current.dKey.isPressed || Keyboard.current.rightArrowKey.isPressed)
                moveInput += 1f;

            isGrounded = CheckGrounded();

            if (Keyboard.current.spaceKey.wasPressedThisFrame && isGrounded)
            {
                rb.linearVelocity = new Vector3(rb.linearVelocity.x, jumpForce, 0f);

                if (!jumpSound.IsNull && AudioManager.instance != null)
                {
                    AudioManager.instance.PlayOneShot(jumpSound, transform.position);
                }
            }
        }
    }

    private bool CheckGrounded()
    {
        if (groundCheck == null) return false;

        Collider[] hits = Physics.OverlapSphere(groundCheck.position, groundCheckRadius, groundLayer);
        foreach (Collider hit in hits)
        {
            if (hit.isTrigger) continue;
            if (hit.transform.root == transform.root) continue;

            return true;
        }
        return false;
    }

    private void FixedUpdate()
    {
        rb.linearVelocity = new Vector3(moveInput * moveSpeed, rb.linearVelocity.y, 0f);
    }

    private void OnDrawGizmosSelected()
    {
        if (groundCheck == null) return;
        Gizmos.color = Color.yellow;
        Gizmos.DrawWireSphere(groundCheck.position, groundCheckRadius);
    }
}